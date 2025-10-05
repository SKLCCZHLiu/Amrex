#include <AMReX_ParallelDescriptor.H>
#include <AMReX_ParmParse.H>
#include <AMReX_MultiFabUtil.H>
#include <AMReX_PlotFileUtil.H>
#include <AMReX_VisMF.H>
#include <AMReX_PhysBCFunct.H>
#include <AMReX_MFIter.H>
#include <AMReX_ParIter.H>

#ifdef AMREX_MEM_PROFILING
#include <AMReX_MemProfiler.H>
#endif

#include "AmrCoreLBM.H"
#include "Kernels.H"
#include "InitParticles.H"

using namespace amrex;

template <class T>
amrex::Gpu::DeviceVector<T>
convertToDeviceVector(amrex::Vector<T> v)
{
    int ncomp = v.size();
    amrex::Gpu::DeviceVector<T> v_d(ncomp);
#ifdef AMREX_USE_GPU
    amrex::Gpu::htod_memcpy(v_d.data(), v.data(), sizeof(T) * ncomp);
#else
    std::memcpy(v_d.data(), v.data(), sizeof(T) * ncomp);
#endif
    return v_d;
}

//********************************************************************//
//                           constructor                              //
//********************************************************************//
AmrCoreLBM::AmrCoreLBM(amrex::Geometry const &level_0_geom, amrex::AmrInfo const &amr_info) : AmrCore(level_0_geom, amr_info)
{
    ReadParameters();

    int nlevs_max = max_level + 1;

    f_new.resize(nlevs_max);
    f_old.resize(nlevs_max);

    velocity.resize(nlevs_max);
    vorticity.resize(nlevs_max);
    shear.resize(nlevs_max);
    density.resize(nlevs_max);
    force.resize(nlevs_max);

    tau.resize(nlevs_max);
    tau[0] = tau_0;

    for (int lev = 1; lev <= max_level; ++lev)
    {
        tau[lev] = 2 * (tau[lev - 1] - 0.5) + 0.5;
    }

    int bc_lo[] = {BCType::foextrap, BCType::foextrap, BCType::foextrap};
    int bc_hi[] = {BCType::foextrap, BCType::foextrap, BCType::foextrap};

    bcs.resize(Q);

    for (int idim = 0; idim < AMREX_SPACEDIM; idim++)
    {
        for (int comp = 0; comp < Q; ++comp)
        {
            bcs[comp].setLo(idim, bc_lo[idim]);
            bcs[comp].setHi(idim, bc_hi[idim]);
        }
    }

    static_lo.resize(2 * nlevs_max);
    static_hi.resize(2 * nlevs_max);

    // 多颗粒生成
    particles.resize(particle_num);
    points.resize(particle_num);
    // generateSpheres(particle_num, R, NX, NY, NZ, points);
    // generateSpheres(D, NX, NY, NZ, points);
    points[0] = {X, Y, Z};
    // points[1] = {X + 2.0*D, Y, Z};
    // points[0] = {X + 0.03*D, Y + 0.03*D, 21.00*D};
    // points[1] = {X - 0.03*D, Y - 0.03*D, 18.96*D};

    // 表面压力系数容器
    particlesCp.resize(particle_num);

    for (int lev = 0; lev <= max_level; lev++)
    {
        amrex::Real dx = Geom(lev).CellSizeArray()[0];

        for (int idim = 0; idim < AMREX_SPACEDIM; idim++)
        {
            static_lo[lev][idim] = 0 + (8); // 边界加密
            static_hi[lev][idim] = Geom(lev).Domain().length(idim) - (8);
        }

        // static_lo[lev+nlevs_max][0] = (center[0] + obj_lo_x - 0.2  * (obj_hi_x - obj_lo_x)) * dx_0 / dx;
        // static_lo[lev+nlevs_max][1] = (center[1] + obj_lo_y - 0.2  * (obj_hi_y - obj_lo_y)) * dx_0 / dx;
        // static_lo[lev+nlevs_max][2] = (center[2] + obj_lo_z - 0.0  * (obj_hi_z - obj_lo_z)) * dx_0 / dx;

        // static_hi[lev+nlevs_max][0] = (center[0] + obj_hi_x + 0.5  * (obj_hi_x - obj_lo_x)) * dx_0 / dx;
        // static_hi[lev+nlevs_max][1] = (center[1] + obj_hi_y + 0.2  * (obj_hi_y - obj_lo_y)) * dx_0 / dx;
        // static_hi[lev+nlevs_max][2] = (center[2] + obj_hi_z + 0.5  * (obj_hi_z - obj_lo_z)) * dx_0 / dx;

        // if(lev == 0)
        // {
        //     static_lo[lev+nlevs_max][0] = (center[0]- 1.5 * D) * dx_0 / dx;
        //     static_lo[lev+nlevs_max][1] = (center[1]- 2.0 * D) * dx_0 / dx;
        //     static_lo[lev+nlevs_max][2] = (center[2]- 2.0 * D) * dx_0 / dx;

        //     static_hi[lev+nlevs_max][0] = (center[0] + 5.0 * D) * dx_0 / dx;
        //     static_hi[lev+nlevs_max][1] = (center[1] + 2.0 * D) * dx_0 / dx;
        //     static_hi[lev+nlevs_max][2] = (center[2] + 2.0 * D) * dx_0 / dx;
        // }
        // if(lev == 1)
        // {
        //     static_lo[lev+nlevs_max][0] = (center[0]- 1.2 * D) * dx_0 / dx;
        //     static_lo[lev+nlevs_max][1] = (center[1]- 1.5 * D) * dx_0 / dx;
        //     static_lo[lev+nlevs_max][2] = (center[2]- 1.5 * D) * dx_0 / dx;

        //     static_hi[lev+nlevs_max][0] = (center[0] + 4.0 * D) * dx_0 / dx;
        //     static_hi[lev+nlevs_max][1] = (center[1] + 1.5 * D) * dx_0 / dx;
        //     static_hi[lev+nlevs_max][2] = (center[2] + 1.5 * D) * dx_0 / dx;
        // }
        // if(lev == 2)
        // {
        //     static_lo[lev+nlevs_max][0] = (center[0]- 1.0 * D) * dx_0 / dx;
        //     static_lo[lev+nlevs_max][1] = (center[1]- 1.0 * D) * dx_0 / dx;
        //     static_lo[lev+nlevs_max][2] = (center[2]- 1.0 * D) * dx_0 / dx;

        //     static_hi[lev+nlevs_max][0] = (center[0] + 3.0 * D) * dx_0 / dx;
        //     static_hi[lev+nlevs_max][1] = (center[1] + 1.0 * D) * dx_0 / dx;
        //     static_hi[lev+nlevs_max][2] = (center[2] + 1.0 * D) * dx_0 / dx;
        // }
        // if(lev == 3)
        // {
        //     static_lo[lev+nlevs_max][0] = (center[0]- 0.7 * D) * dx_0 / dx;
        //     static_lo[lev+nlevs_max][1] = (center[1]- 0.8 * D) * dx_0 / dx;
        //     static_lo[lev+nlevs_max][2] = (center[2]- 0.8 * D) * dx_0 / dx;

        //     static_hi[lev+nlevs_max][0] = (center[0] + 1.5 * D) * dx_0 / dx;
        //     static_hi[lev+nlevs_max][1] = (center[1] + 0.8 * D) * dx_0 / dx;
        //     static_hi[lev+nlevs_max][2] = (center[2] + 0.8 * D) * dx_0 / dx;
        // }
        // if(lev == 4)
        // {
        //     static_lo[lev+nlevs_max][0] = (center[0]- 2.0 * D) * dx_0 / dx;
        //     static_lo[lev+nlevs_max][1] = (center[1]- 2.0 * D) * dx_0 / dx;
        //     static_lo[lev+nlevs_max][2] = (center[2]- 2.0 * D) * dx_0 / dx;

        //     static_hi[lev+nlevs_max][0] = (center[0] + 4.0 * D) * dx_0 / dx;
        //     static_hi[lev+nlevs_max][1] = (center[1] + 2.0 * D) * dx_0 / dx;
        //     static_hi[lev+nlevs_max][2] = (center[2] + 2.0 * D) * dx_0 / dx;
        // }
    }
}

AmrCoreLBM::AmrCoreLBM()
{
}
AmrCoreLBM::~AmrCoreLBM() = default;

//********************************************************************//
//                           help function                            //
//********************************************************************//
void AmrCoreLBM::PrintMeshInfo()
{
    if (ParallelDescriptor::IOProcessor())
    {
        amrex::Print() << "╔════════════════════════════════════════════════════════╗" << std::endl;
        amrex::Print() << "║               Mesh Information                         ║" << std::endl;
        amrex::Print() << "╚════════════════════════════════════════════════════════╝" << std::endl;

        printGridSummary(amrex::OutStream(), 0, finest_level);
        for (int i = 0; i <= finest_level; ++i)
        {
            amrex::Print() << std::setw(15) << std::left << "  blocking_factor[" << i << "]"
                           << std::setw(10) << std::right << blocking_factor[i] << std::endl;
        }
        for (int i = 0; i <= finest_level; ++i)
        {
            amrex::Print() << std::setw(15) << std::left << "  max_grid_size[" << i << "]  "
                           << std::setw(10) << std::right << max_grid_size[i] << std::endl;
        }
        for (int i = 0; i <= finest_level; ++i)
        {
            amrex::Print() << std::setw(15) << std::left << "  n_error_buf[" << i << "]    "
                           << std::setw(10) << std::right << n_error_buf[i] << std::endl;
        }
        amrex::Print() << std::setw(15) << std::left << "  grid_eff       " << std::setw(10) << std::right << grid_eff << std::endl;
        amrex::Print() << "╚════════════════════════════════════════════════════════╝" << std::endl;
        amrex::Print() << std::endl;
    }
}
void AmrCoreLBM::PrintLbmParm()
{
    amrex::Print() << "╔════════════════════════════════════════════════════════╗" << std::endl;
    amrex::Print() << "║               LBM Parameters                           ║" << std::endl;
    amrex::Print() << "╚════════════════════════════════════════════════════════╝" << std::endl;

    amrex::Print() << std::setw(15) << std::left << "  NX     =" << std::setw(10) << std::right << Geom(0).Domain().length(0) << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  NY     =" << std::setw(10) << std::right << Geom(0).Domain().length(1) << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  NZ     =" << std::setw(10) << std::right << Geom(0).Domain().length(2) << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  dx_0   =" << std::setw(10) << std::right << dx_0 << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  dx_min =" << std::setw(10) << std::right << dx_min << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  Re     =" << std::setw(10) << std::right << Re << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  cs2    =" << std::setw(10) << std::right << cs2 << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  p0     =" << std::setw(10) << std::right << p0 << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  Ma     =" << std::setw(10) << std::right << Ma << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  U0     =" << std::setw(10) << std::right << Uc << std::endl;

    for (int lev = 0; lev <= finest_level; lev++)
    {
        amrex::Print() << std::setw(15) << std::left << "  tau    =" << std::setw(10) << std::right << tau[lev] << std::endl;
    }

    amrex::Print() << "╚════════════════════════════════════════════════════════╝" << std::endl;
    amrex::Print() << std::endl;
}
void AmrCoreLBM::ReadParameters()
{
    {
        ParmParse pp("amr");
        pp.query("plot_file", plot_file);
        pp.query("grid_eff", grid_eff);

        // Read in the n_error_buf
        int cnt = pp.countval("n_error_buf");
        if (cnt > 0)
        {
            Vector<int> neb;
            pp.getarr("n_error_buf", neb);
            int n = std::min(cnt, max_level + 1);
            for (int i = 0; i < n; ++i)
            {
                n_error_buf[i] = IntVect(neb[i]);
            }
            for (int i = n; i <= max_level; ++i)
            {
                n_error_buf[i] = IntVect(neb[cnt - 1]);
            }
        }

        cnt = pp.countval("n_error_buf_x");
        if (cnt > 0)
        {
            int idim = 0;
            Vector<int> neb;
            pp.getarr("n_error_buf_x", neb);
            int n = std::min(cnt, max_level + 1);
            for (int i = 0; i < n; ++i)
            {
                n_error_buf[i][idim] = neb[i];
            }
            for (int i = n; i <= max_level; ++i)
            {
                n_error_buf[i][idim] = neb[n - 1];
            }
        }

#if (AMREX_SPACEDIM > 1)
        cnt = pp.countval("n_error_buf_y");
        if (cnt > 0)
        {
            int idim = 1;
            Vector<int> neb;
            pp.getarr("n_error_buf_y", neb);
            int n = std::min(cnt, max_level + 1);
            for (int i = 0; i < n; ++i)
            {
                n_error_buf[i][idim] = neb[i];
            }
            for (int i = n; i <= max_level; ++i)
            {
                n_error_buf[i][idim] = neb[n - 1];
            }
        }
#endif

#if (AMREX_SPACEDIM == 3)
        cnt = pp.countval("n_error_buf_z");
        if (cnt > 0)
        {
            int idim = 2;
            Vector<int> neb;
            pp.getarr("n_error_buf_z", neb);
            int n = std::min(cnt, max_level + 1);
            for (int i = 0; i < n; ++i)
            {
                n_error_buf[i][idim] = neb[i];
            }
            for (int i = n; i <= max_level; ++i)
            {
                n_error_buf[i][idim] = neb[n - 1];
            }
        }
#endif

        // Read in the max_grid_size
        cnt = pp.countval("max_grid_size");
        if (cnt > 0)
        {
            Vector<int> mgs;
            pp.getarr("max_grid_size", mgs);
            int last_mgs = mgs.back();
            mgs.resize(max_level + 1, last_mgs);
            SetMaxGridSize(mgs);
        }

        cnt = pp.countval("max_grid_size_x");
        if (cnt > 0)
        {
            int idim = 0;
            Vector<int> mgs;
            pp.getarr("max_grid_size_x", mgs);
            int n = std::min(cnt, max_level + 1);
            for (int i = 0; i < n; ++i)
            {
                max_grid_size[i][idim] = mgs[i];
            }
            for (int i = n; i <= max_level; ++i)
            {
                max_grid_size[i][idim] = mgs[n - 1];
            }
        }

#if (AMREX_SPACEDIM > 1)
        cnt = pp.countval("max_grid_size_y");
        if (cnt > 0)
        {
            int idim = 1;
            Vector<int> mgs;
            pp.getarr("max_grid_size_y", mgs);
            int n = std::min(cnt, max_level + 1);
            for (int i = 0; i < n; ++i)
            {
                max_grid_size[i][idim] = mgs[i];
            }
            for (int i = n; i <= max_level; ++i)
            {
                max_grid_size[i][idim] = mgs[n - 1];
            }
        }
#endif

#if (AMREX_SPACEDIM == 3)
        cnt = pp.countval("max_grid_size_z");
        if (cnt > 0)
        {
            int idim = 2;
            Vector<int> mgs;
            pp.getarr("max_grid_size_z", mgs);
            int n = std::min(cnt, max_level + 1);
            for (int i = 0; i < n; ++i)
            {
                max_grid_size[i][idim] = mgs[i];
            }
            for (int i = n; i <= max_level; ++i)
            {
                max_grid_size[i][idim] = mgs[n - 1];
            }
        }
#endif

        // Read in the blocking_factors
        cnt = pp.countval("blocking_factor");
        if (cnt > 0)
        {
            Vector<int> bf;
            pp.getarr("blocking_factor", bf);
            int last_bf = bf.back();
            bf.resize(max_level + 1, last_bf);
            SetBlockingFactor(bf);
        }

        cnt = pp.countval("blocking_factor_x");
        if (cnt > 0)
        {
            int idim = 0;
            Vector<int> bf;
            pp.getarr("blocking_factor_x", bf);
            int n = std::min(cnt, max_level + 1);
            for (int i = 0; i < n; ++i)
            {
                blocking_factor[i][idim] = bf[i];
            }
            for (int i = n; i <= max_level; ++i)
            {
                blocking_factor[i][idim] = bf[n - 1];
            }
        }

#if (AMREX_SPACEDIM > 1)
        cnt = pp.countval("blocking_factor_y");
        if (cnt > 0)
        {
            int idim = 1;
            Vector<int> bf;
            pp.getarr("blocking_factor_y", bf);
            int n = std::min(cnt, max_level + 1);
            for (int i = 0; i < n; ++i)
            {
                blocking_factor[i][idim] = bf[i];
            }
            for (int i = n; i <= max_level; ++i)
            {
                blocking_factor[i][idim] = bf[n - 1];
            }
        }
#endif

#if (AMREX_SPACEDIM == 3)
        cnt = pp.countval("blocking_factor_z");
        if (cnt > 0)
        {
            int idim = 2;
            Vector<int> bf;
            pp.getarr("blocking_factor_z", bf);
            int n = std::min(cnt, max_level + 1);
            for (int i = 0; i < n; ++i)
            {
                blocking_factor[i][idim] = bf[i];
            }
            for (int i = n; i <= max_level; ++i)
            {
                blocking_factor[i][idim] = bf[n - 1];
            }
        }
#endif
    }

    {
        ParmParse pp("lbm");
        int n = pp.countval("err");
        if (n > 0)
        {
            pp.getarr("err", err, 0, n);
        }
    }
}

void AmrCoreLBM::WriteVelocityFile(const int step, const amrex::Real time)
{
    const std::string &plotfilename = amrex::Concatenate(plot_file, step, 6);

    amrex::Vector<const amrex::MultiFab *> mf;

    for (int i = 0; i <= finest_level; ++i)
    {
        mf.push_back(&velocity[i]);
    }

    amrex::Vector<std::string> varnames = {"ux", "uy", "uz"};

    amrex::WriteMultiLevelPlotfile(plotfilename, finest_level + 1, mf, varnames,
                                   Geom(), time, Vector<int>(finest_level + 1, step), refRatio());
}

void AmrCoreLBM::WriteDensityFile(const int step, const amrex::Real time)
{
    std::string plot_file_density{plot_file + "density_"};
    const std::string &plotfilename = amrex::Concatenate(plot_file_density, step, 6);

    amrex::Vector<const amrex::MultiFab *> mf;

    for (int i = 0; i <= finest_level; ++i)
    {
        mf.push_back(&density[i]);
    }

    amrex::Vector<std::string> varnames = {"rho"};

    amrex::WriteMultiLevelPlotfile(plotfilename, finest_level + 1, mf, varnames,
                                   Geom(), time, Vector<int>(finest_level + 1, step), refRatio());
}

void AmrCoreLBM::WriteVelocityFile(const int step, const amrex::Real time, const int lev)
{
    const std::string &plotfilename = amrex::Concatenate(plot_file, step, 6);

    amrex::Vector<const amrex::MultiFab *> mf;

    mf.push_back(&velocity[lev]);

    amrex::Vector<std::string> varnames = {"ux", "uy", "uz"};

    amrex::WriteMultiLevelPlotfile(plotfilename, 1, mf, varnames,
                                   Geom(), time, Vector<int>(1, step), refRatio());
}

void AmrCoreLBM::WriteVorticityFile(const int step, const amrex::Real time)
{
    std::string plot_file_vort{plot_file + "vort_"};
    const std::string &plotfilename = amrex::Concatenate(plot_file_vort, step, 6);

    amrex::Vector<const amrex::MultiFab *> mf;

    for (int i = 0; i <= finest_level; ++i)
    {
        mf.push_back(&vorticity[i]);
    }

    amrex::Vector<std::string> varnames = {"Q", "Vorticity"};

    amrex::WriteMultiLevelPlotfile(plotfilename, finest_level + 1, mf, varnames,
                                   Geom(), time, Vector<int>(finest_level + 1, step), refRatio());
}

void AmrCoreLBM::WriteParticleFile(const int step, const amrex::Real time)
{
    for (int i = 0; i < particle_num; i++)
    {
        particles[i]->WriteParticle(step);
    }
}

void AmrCoreLBM::WriteMultiParticleFile(const int step, const amrex::Real time)
{
    if (ParallelDescriptor::MyProc() == ParallelDescriptor::IOProcessorNumber())
    {
        std::string filename = amrex::Concatenate("particle_data", step, 6) + ".dat";

        // 打开文件并检查是否成功
        std::ofstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Cannot open the file: " << filename << std::endl;
            return;
        }

        // 写入文件内容
        file << "TITLE=particle_data\n";
        file << "VARIABLES=x,y,z,id\n";
        file << "Zone I= " << particle_num << ", J= " << AMREX_SPACEDIM + 1 << ", F= POINT\n";

        file << std::fixed << std::setprecision(6);

        // 写入粒子的坐标数据
        for (int j = 0; j < particle_num; ++j)
        {
            file << points[j][0] << "\t" << points[j][1] << "\t" << points[j][2] << "\t" << j << "\n";
        }

        file.close();
    }
}

//********************************************************************//
//                           mesh function                            //
//********************************************************************//
void AmrCoreLBM::InitMesh(amrex::Real cur_time)
{
    InitFromScratch(cur_time);
}
void AmrCoreLBM::FillCoarsePatch(int lev, amrex::Real time, amrex::MultiFab &mf) // 根本没有用到
{
    // amrex::AllPrint()<<"FillCoarse Patch from " << lev-1 << " to " << lev <<std::endl;

    Interpolater *mapper = &cell_cons_interp;

    if (Gpu::inLaunchRegion())
    {
        GpuBndryFuncFab<AmrCoreFill> gpu_bndry_func(AmrCoreFill{});
        PhysBCFunct<GpuBndryFuncFab<AmrCoreFill>> cphysbc(geom[lev - 1], bcs, gpu_bndry_func);
        PhysBCFunct<GpuBndryFuncFab<AmrCoreFill>> fphysbc(geom[lev], bcs, gpu_bndry_func);

        amrex::InterpFromCoarseLevel(mf, time, f_old[lev - 1], 0, 0, Q, geom[lev - 1], geom[lev],
                                     cphysbc, 0, fphysbc, 0, refRatio(lev - 1),
                                     mapper, bcs, 0);
    }
    else
    {
        CpuBndryFuncFab bndry_func(nullptr); // Without EXT_DIR, we can pass a nullptr.
        PhysBCFunct<CpuBndryFuncFab> cphysbc(geom[lev - 1], bcs, bndry_func);
        PhysBCFunct<CpuBndryFuncFab> fphysbc(geom[lev], bcs, bndry_func);

        amrex::InterpFromCoarseLevel(mf, time, f_old[lev - 1], 0, 0, Q, geom[lev - 1], geom[lev],
                                     cphysbc, 0, fphysbc, 0, refRatio(lev - 1),
                                     mapper, bcs, 0);
    }
}
void AmrCoreLBM::FillPatch(int lev, amrex::Real time, amrex::MultiFab &mf)
{
    // amrex::AllPrint()<<"FillPatch from " << lev-1 << " to " << lev <<std::endl;

    Interpolater *mapper = &cell_cons_interp;

    if (lev == 0)
    {
        amrex::MultiFab &f_old_lev = f_old[lev];
        amrex::Vector<amrex::MultiFab *> cmf{&f_old_lev};
        amrex::Vector<Real> ctime{time};

        if (Gpu::inLaunchRegion())
        {
            GpuBndryFuncFab<AmrCoreFill> gpu_bndry_func(AmrCoreFill{});
            PhysBCFunct<GpuBndryFuncFab<AmrCoreFill>> physbc(geom[lev], bcs, gpu_bndry_func);
            FillPatchSingleLevel(mf, time, cmf, ctime, 0, 0, Q, geom[lev], physbc, 0);
        }
        else
        {
            CpuBndryFuncFab bndry_func(nullptr);
            PhysBCFunct<CpuBndryFuncFab> physbc(geom[lev], bcs, bndry_func);
            FillPatchSingleLevel(mf, time, cmf, ctime, 0, 0, Q, geom[lev], physbc, 0);
        }
    }
    else
    {
        amrex::MultiFab &f_old_lev_c = f_old[lev - 1];
        amrex::MultiFab &f_old_lev_f = f_old[lev];

        amrex::Vector<amrex::MultiFab *> cmf{&f_old_lev_c};
        amrex::Vector<amrex::MultiFab *> fmf{&f_old_lev_f};

        amrex::Vector<Real> ctime{time};
        amrex::Vector<Real> ftime{time};

        if (Gpu::inLaunchRegion())
        {
            GpuBndryFuncFab<AmrCoreFill> gpu_bndry_func(AmrCoreFill{});
            PhysBCFunct<GpuBndryFuncFab<AmrCoreFill>> cphysbc(geom[lev - 1], bcs, gpu_bndry_func);
            PhysBCFunct<GpuBndryFuncFab<AmrCoreFill>> fphysbc(geom[lev], bcs, gpu_bndry_func);
            amrex::FillPatchTwoLevels(mf, time, cmf, ctime, fmf, ftime, 0, 0, Q,
                                      geom[lev - 1], geom[lev], cphysbc, 0, fphysbc, 0,
                                      refRatio(lev - 1), mapper, bcs, 0);
        }
        else
        {
            CpuBndryFuncFab bndry_func(nullptr);
            PhysBCFunct<CpuBndryFuncFab> cphysbc(geom[lev - 1], bcs, bndry_func);
            PhysBCFunct<CpuBndryFuncFab> fphysbc(geom[lev], bcs, bndry_func);

            amrex::FillPatchTwoLevels(mf, time, cmf, ctime, fmf, ftime, 0, 0, Q,
                                      geom[lev - 1], geom[lev], cphysbc, 0, fphysbc, 0,
                                      refRatio(lev - 1), mapper, bcs, 0);
        }
    }
}

void AmrCoreLBM::FillDdfPatch(int lev, amrex::Real time, amrex::MultiFab &mf) // 加入缩放
{
    // amrex::AllPrint()<<"FillDdfPatch from " << lev-1 << " to " << lev <<std::endl;

    Interpolater *mapper = &cell_cons_interp;

    amrex::MultiFab &f_new_lev_c = f_new[lev - 1]; // 用f_new当缓存容器
    amrex::MultiFab &f_old_lev_f = f_old[lev];     // 保持和传入的mf一致

    amrex::Vector<amrex::MultiFab *> cmf{&f_new_lev_c};
    amrex::Vector<amrex::MultiFab *> fmf{&f_old_lev_f};

    amrex::Vector<Real> ctime{time};
    amrex::Vector<Real> ftime{time};

    // 如果是粗网格插值到细网格valid,无论如何只需要操作粗网格就可以了。
    amrex::MultiFab &f_old_lev_c = f_old[lev - 1];
    amrex::Real scale = tau[lev] / tau[lev - 1] / 2.0;

    for (MFIter mfi(f_old_lev_c, TilingIfNotGPU()); mfi.isValid(); ++mfi)
    {
        const auto bx = mfi.growntilebox(0); // 只需要粗网格的valid值就可以了

        const Array4<Real> &fold = f_old_lev_c.array(mfi);
        const Array4<Real> &fnew = f_new_lev_c.array(mfi);

        amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k)
                           { interp_scale(i, j, k, fold, fnew, scale); });
    }

    if (Gpu::inLaunchRegion())
    {
        GpuBndryFuncFab<AmrCoreFill> gpu_bndry_func(AmrCoreFill{});
        PhysBCFunct<GpuBndryFuncFab<AmrCoreFill>> cphysbc(geom[lev - 1], bcs, gpu_bndry_func);
        PhysBCFunct<GpuBndryFuncFab<AmrCoreFill>> fphysbc(geom[lev], bcs, gpu_bndry_func);
        amrex::FillPatchTwoLevels(mf, time, cmf, ctime, fmf, ftime, 0, 0, Q,
                                  geom[lev - 1], geom[lev], cphysbc, 0, fphysbc, 0,
                                  refRatio(lev - 1), mapper, bcs, 0);
    }
    else
    {
        CpuBndryFuncFab bndry_func(nullptr);
        PhysBCFunct<CpuBndryFuncFab> cphysbc(geom[lev - 1], bcs, bndry_func);
        PhysBCFunct<CpuBndryFuncFab> fphysbc(geom[lev], bcs, bndry_func);

        amrex::FillPatchTwoLevels(mf, time, cmf, ctime, fmf, ftime, 0, 0, Q,
                                  geom[lev - 1], geom[lev], cphysbc, 0, fphysbc, 0,
                                  refRatio(lev - 1), mapper, bcs, 0); // 如果time与ftime匹配,则会把细网格覆盖过去。
    }
}

void AmrCoreLBM::FillMacroPatch(int lev, amrex::Real time, amrex::MultiFab &mf)
{
    Interpolater *mapper = &cell_cons_interp;

    if (lev == 0)
    {
        amrex::MultiFab &u_lev = velocity[lev];
        amrex::Vector<amrex::MultiFab *> cmf{&u_lev};
        amrex::Vector<Real> ctime{time};

        if (Gpu::inLaunchRegion())
        {
            GpuBndryFuncFab<AmrCoreFill> gpu_bndry_func(AmrCoreFill{});
            PhysBCFunct<GpuBndryFuncFab<AmrCoreFill>> physbc(geom[lev], bcs, gpu_bndry_func);
            FillPatchSingleLevel(mf, time, cmf, ctime, 0, 0, AMREX_SPACEDIM, geom[lev], physbc, 0);
        }
        else
        {
            CpuBndryFuncFab bndry_func(nullptr);
            PhysBCFunct<CpuBndryFuncFab> physbc(geom[lev], bcs, bndry_func);
            FillPatchSingleLevel(mf, time, cmf, ctime, 0, 0, AMREX_SPACEDIM, geom[lev], physbc, 0);
        }
    }
    else
    {
        amrex::MultiFab &u_lev_c = velocity[lev - 1];
        amrex::MultiFab &u_lev_f = velocity[lev];

        amrex::Vector<amrex::MultiFab *> cmf{&u_lev_c};
        amrex::Vector<amrex::MultiFab *> fmf{&u_lev_f};

        amrex::Vector<Real> ctime{time};
        amrex::Vector<Real> ftime{time};

        if (Gpu::inLaunchRegion())
        {
            GpuBndryFuncFab<AmrCoreFill> gpu_bndry_func(AmrCoreFill{});
            PhysBCFunct<GpuBndryFuncFab<AmrCoreFill>> cphysbc(geom[lev - 1], bcs, gpu_bndry_func);
            PhysBCFunct<GpuBndryFuncFab<AmrCoreFill>> fphysbc(geom[lev], bcs, gpu_bndry_func);
            amrex::FillPatchTwoLevels(mf, time, cmf, ctime, fmf, ftime, 0, 0, AMREX_SPACEDIM,
                                      geom[lev - 1], geom[lev], cphysbc, 0, fphysbc, 0,
                                      refRatio(lev - 1), mapper, bcs, 0);
        }
        else
        {
            CpuBndryFuncFab bndry_func(nullptr);
            PhysBCFunct<CpuBndryFuncFab> cphysbc(geom[lev - 1], bcs, bndry_func);
            PhysBCFunct<CpuBndryFuncFab> fphysbc(geom[lev], bcs, bndry_func);

            amrex::FillPatchTwoLevels(mf, time, cmf, ctime, fmf, ftime, 0, 0, AMREX_SPACEDIM,
                                      geom[lev - 1], geom[lev], cphysbc, 0, fphysbc, 0,
                                      refRatio(lev - 1), mapper, bcs, 0);
        }
    }
}

void AmrCoreLBM::RefineMesh(amrex::Real cur_time)
{
    // amrex::Print()<<"..............."<<std::endl;
    // amrex::Print()<<"regrid begin..."<<std::endl;
    regrid(0, cur_time);
    // amrex::Print()<<"regrid end....."<<std::endl;
    // amrex::Print()<<"..............."<<std::endl;
}

void AmrCoreLBM::FindCentre()
{
    // amrex::AllPrint()<<"FindCentre "<<std::endl;

    for (int p_num = 0; p_num < particle_num; p_num++)
    {
        points[p_num] = particles[p_num]->ReturnCentre();
        // amrex::Print() << "id " << p_num << "'s z_positon is " << points[p_num][2] << std::endl;
    }
}

//********************************************************************//
//                           lbm  function                            //
//********************************************************************//

void AmrCoreLBM::ComputeMacroLevel(int lev)
{
    amrex::MultiFab &f_old_lev = f_old[lev];
    amrex::MultiFab &rho_lev = density[lev];
    amrex::MultiFab &u_lev = velocity[lev];

    for (MFIter mfi(f_old_lev, TilingIfNotGPU()); mfi.isValid(); ++mfi)
    {
        const Box &bx = mfi.growntilebox(nghost);
        Array4<Real> const &fold = f_old_lev.array(mfi);
        Array4<Real> const &rho = rho_lev.array(mfi);
        Array4<Real> const &u = u_lev.array(mfi);

        amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k)
                           { compute_macro(i, j, k, fold, rho, u); });
    }
}

void AmrCoreLBM::ComputeMacro()
{
    for (int lev = 0; lev <= finest_level; lev++)
    {
        ComputeMacroLevel(lev);
    }
}

void AmrCoreLBM::ComputeVorticityLevel(int lev)
{
    // amrex::AllPrint()<<"ComputeVorticityLevel on " << lev <<std::endl;

    amrex::MultiFab &f_old_lev = f_old[lev];
    amrex::MultiFab &u_lev = velocity[lev];
    amrex::MultiFab &vort_lev = vorticity[lev];
    amrex::Real dt = Geom(lev).CellSizeArray()[0];

    for (MFIter mfi(f_old_lev, TilingIfNotGPU()); mfi.isValid(); ++mfi)
    {
        const Box &bx = mfi.growntilebox(0);

        Array4<Real> const &u = u_lev.array(mfi);
        Array4<Real> const &vort = vort_lev.array(mfi);

        amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k)
                           { compute_vorticity(i, j, k, u, vort, dt); });
    }
}

void AmrCoreLBM::ComputeVorticity(amrex::Real cur_time)
{
    for (int lev = 0; lev <= finest_level; lev++)
    {
        FillMacroGhostLevel(lev, cur_time);
        ComputeVorticityLevel(lev);
    }
}

void AmrCoreLBM::ComputeShearLevel(int lev)
{
    amrex::MultiFab &f_old_lev = f_old[lev];
    amrex::MultiFab &u_lev = velocity[lev];
    amrex::MultiFab &shear_lev = shear[lev];
    amrex::Real dt = Geom(lev).CellSizeArray()[0];

    for (MFIter mfi(f_old_lev, TilingIfNotGPU()); mfi.isValid(); ++mfi)
    {
        const Box &bx = mfi.growntilebox(0);

        Array4<Real> const &u = u_lev.array(mfi);
        Array4<Real> const &shear = shear_lev.array(mfi);

        amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k)
                           { compute_shear(i, j, k, u, shear, dt); });
    }
}

void AmrCoreLBM::ComputeShear()
{
    for (int lev = 0; lev <= finest_level; lev++)
    {
        ComputeShearLevel(lev);
    }
}

void AmrCoreLBM::AverageDownValidLevel(int lev, bool is_scale)
{
    // amrex::AllPrint()<<"AverageDownValidLevel from " << lev+1 << " to " << lev <<std::endl;
    // amrex::average_down(f_old[lev+1], f_old[lev], geom[lev+1], geom[lev],0, Q, refRatio(lev));

    amrex::MultiFab &fine_mf = f_old[lev + 1];
    amrex::MultiFab &crse_mf = f_old[lev];

    MultiFab fine_boundary_data(fine_mf.boxArray(), fine_mf.DistributionMap(), Q, 0); // 能不能用f_new减少内存消耗
    MultiFab::Copy(fine_boundary_data, fine_mf, 0, 0, Q, 0);

    if (is_scale)
    {
        amrex::Real scale = 2.0 * tau[lev] / tau[lev + 1];

        for (MFIter mfi(fine_boundary_data, TilingIfNotGPU()); mfi.isValid(); ++mfi)
        {
            const auto bx = mfi.growntilebox(0);

            const Array4<Real> &fold = fine_boundary_data.array(mfi);

            amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k)
                               { average_scale(i, j, k, fold, scale); });
        }
    }
    amrex::average_down(fine_boundary_data, crse_mf, 0, Q, refRatio(lev));
}

void AmrCoreLBM::AverageDownValid()
{
    for (int lev = finest_level - 1; lev >= 0; --lev)
    {
        AverageDownValidLevel(lev, 1);
    }
}

// void AmrCoreLBM::AverageDownGhostLevel(int lev)
// {
//     amrex::MultiFab& fine_mf = f_old[lev+1];
//     amrex::MultiFab& crse_mf = f_old[lev];
//     BoxArray fine_boundary_grids;

//     BoxArray const& fbas = fine_mf.boxArray();
//     BoxList bl;

//     for(int i = 0; i < fbas.size(); ++i)
//     {
//         Box const& b = amrex::grow(fbas[i], 2);
//         auto const& bltmp = fbas.complementIn(b);
//         bl.join(bltmp);
//     }

//     fine_boundary_grids.define(std::move(bl));

//     DistributionMapping fine_boundary_dmap(fine_boundary_grids);

//     MultiFab fine_boundary_data(fine_boundary_grids, fine_boundary_dmap, Q, 0);

//     fine_boundary_data.ParallelCopy(fine_mf, 0, 0, Q, 2, 0);

//     amrex::average_down(fine_boundary_data, crse_mf, 0, Q, refRatio(lev));
// }

void AmrCoreLBM::AverageDownGhostLevel(int lev, bool is_scale)
{
    // amrex::AllPrint()<<"AverageDownGhostLevel from " << lev+1 << " to " << lev <<std::endl;

    amrex::MultiFab &fine_mf = f_old[lev + 1];
    amrex::MultiFab &crse_mf = f_old[lev];

    MultiFab fine_boundary_data(fine_mf.boxArray(), fine_mf.DistributionMap(), Q, 2); // 能不能用f_new减少内存消耗
    MultiFab::Copy(fine_boundary_data, fine_mf, 0, 0, Q, 2);

    if (is_scale)
    {
        amrex::Real scale = 2.0 * tau[lev] / tau[lev + 1];

        for (MFIter mfi(fine_boundary_data, TilingIfNotGPU()); mfi.isValid(); ++mfi)
        {
            const auto bx = mfi.growntilebox(2);

            const Array4<Real> &fold = fine_boundary_data.array(mfi);

            amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k)
                               { average_scale(i, j, k, fold, scale); });
        }
    }

    amrex::average_down(fine_boundary_data, crse_mf, 0, Q, refRatio(lev));
}

void AmrCoreLBM::AverageDownGhost()
{
}

void AmrCoreLBM::FillGhostLevel(int lev, amrex::Real time, bool is_scale)
{
    amrex::MultiFab &f_old_lev = f_old[lev];

    if (is_scale)
    {
        FillDdfPatch(lev, time, f_old_lev);
    }
    else
    {
        FillPatch(lev, time, f_old_lev);
    }
}

/**
 * @brief 填充宏观速度场的 ghost cell 层
 * @param lev 网格层级
 * @param time 当前时间
 */
void AmrCoreLBM::FillMacroGhostLevel(int lev, amrex::Real time)
{
    amrex::MultiFab &u_lev = velocity[lev]; // 获取当前层级的速度场多网格数据结构

    if (lev == 0)
    {
        u_lev.FillBoundary(geom[lev].periodicity()); // 对于最底层网格，只填充周期性边界
    }
    else
    {
        FillMacroPatch(lev, time, u_lev);            // 填充c-f边界
        u_lev.FillBoundary(geom[lev].periodicity()); // 填充同等级
    }
}

void AmrCoreLBM::FillForceGhostLevel(int lev, amrex::Real time)
{
    // amrex::AllPrint()<<"FillForceGhostLevel on " << lev <<std::endl;

    amrex::MultiFab &force_lev = force[lev];

    if (lev == 0)
    {
        force_lev.FillBoundary(geom[lev].periodicity());
    }
    else
    {
        // 填充c-f边界
        force_lev.FillBoundary(geom[lev].periodicity()); // 填充同等级
    }
}

void AmrCoreLBM::CommunicateLevel(int lev)
{
    amrex::MultiFab &f_old_lev = f_old[lev];
    f_old_lev.FillBoundary(geom[lev].periodicity());
}

void AmrCoreLBM::Boundary(int lev)
{
    // amrex::AllPrint()<<"Boundary on " << lev <<std::endl;

    int right = Geom(lev).Domain().length(0) - 1;
    int back = Geom(lev).Domain().length(1) - 1;
    int up = Geom(lev).Domain().length(2) - 1;
    amrex::IntVect hi{right, back, up};

    amrex::MultiFab &f_old_lev = f_old[lev];
    amrex::MultiFab &f_new_lev = f_new[lev];

    for (MFIter mfi(f_old_lev, TilingIfNotGPU()); mfi.isValid(); ++mfi)
    {
        const auto bx = mfi.growntilebox(nghost);
        const Array4<Real> &fold = f_old_lev.array(mfi);
        const Array4<Real> &fnew = f_new_lev.array(mfi);

        amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k)
                           { fill_boundary(i, j, k, fold, fnew, hi); });
    }
}

void AmrCoreLBM::Collide(int lev, int n)
{
    // amrex::AllPrint()<<"Collide on " << lev <<std::endl;

    int right = Geom(lev).Domain().length(0) - 1;
    int back = Geom(lev).Domain().length(1) - 1;
    int up = Geom(lev).Domain().length(2) - 1;
    amrex::IntVect hi{right, back, up};

    amrex::MultiFab &f_old_lev = f_old[lev];
    amrex::MultiFab &shear_lev = shear[lev];
    amrex::MultiFab &force_lev = force[lev];
    amrex::Real dt = Geom(lev).CellSizeArray()[0];
    amrex::Real tau_lev = tau[lev];

    for (MFIter mfi(f_old_lev, TilingIfNotGPU()); mfi.isValid(); ++mfi)
    {
        const auto bx = mfi.growntilebox(n);
        const Array4<Real> &fold = f_old_lev.array(mfi);
        const Array4<Real> &s = shear_lev.array(mfi);
        const Array4<Real> &Ft = force_lev.array(mfi);

        amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k)
                           {
                               // collide(i, j, k, fold, s, Ft, tau_lev, dt, hi);
                               collide_cumulant(i, j, k, fold, s, Ft, tau_lev, dt, hi);
                               // collide_cumulant_opt(i, j, k, fold, s, Ft, tau_lev, dt, hi);
                               // collide_cumulant_opt2(i, j, k, fold, s, Ft, tau_lev, dt, hi);
                           });
    }
}

void AmrCoreLBM::Stream(int lev, int n)
{
    // amrex::AllPrint()<<"Stream on " << lev <<std::endl;

    int right = Geom(lev).Domain().length(0) - 1;
    int back = Geom(lev).Domain().length(1) - 1;
    int up = Geom(lev).Domain().length(2) - 1;
    amrex::IntVect hi{right, back, up};

    amrex::MultiFab &f_old_lev = f_old[lev];
    amrex::MultiFab &f_new_lev = f_new[lev];

    bool is_finest = {lev == finest_level};
    bool is_coarsest = {lev == 0};

    for (MFIter mfi(f_old_lev, TilingIfNotGPU()); mfi.isValid(); ++mfi)
    {
        const auto bx = mfi.growntilebox(n);
        const Array4<Real> &fold = f_old_lev.array(mfi);
        const Array4<Real> &fnew = f_new_lev.array(mfi);

        amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k)
                           { stream(i, j, k, fold, fnew, hi, is_finest); });
    }
}

// void AmrCoreLBM::SwapLevel(int lev, int n)
// {
//     amrex::MultiFab& f_old_lev = f_old[lev];
//     amrex::MultiFab& f_new_lev = f_new[lev];

//     for(MFIter mfi(f_old_lev, TilingIfNotGPU()); mfi.isValid(); ++mfi)
//     {
//         const auto bx = mfi.growntilebox(n);
//         const Array4<Real>& fold = f_old_lev.array(mfi);
//         const Array4<Real>& fnew = f_new_lev.array(mfi);

//         amrex::ParallelFor(bx, [=]AMREX_GPU_DEVICE(int i, int j, int k)
//         {
//             swap_ddf(i, j, k, fold, fnew);
//         });
//     }
// }

void AmrCoreLBM::SwapLevel(int lev, int n)
{
    amrex::MultiFab &f_old_lev = f_old[lev];
    amrex::MultiFab &f_new_lev = f_new[lev];

    std::swap(f_old_lev, f_new_lev);
}

void AmrCoreLBM::InterpScale(int lev, int n) // n=nghost
{
    amrex::AllPrint() << "InterpScale not existing !" << std::endl;
}

void AmrCoreLBM::AverageScale(int lev, int n)
{
    amrex::AllPrint() << "AverageScale not existing !" << std::endl;
}

//********************************************************************//
//                           ibm  function                            //
//********************************************************************//
void AmrCoreLBM::InitParticle(int lev)
{
    for (int i = 0; i < particle_num; i++)
    {
        particles[i] = std::make_unique<LagrangeParticleContainer>(this, points[i], i);
        particles[i]->InitParticle(lev);
        // particles[i] ->InitParticleFromFile(lev, "../../../object/sphere128");
        // particles[i] ->InitParticleFromFile(lev, "../../../object/car_fine");
    }
}

void AmrCoreLBM::InterpForce(int lev)
{
    amrex::MultiFab &rho_lev = density[lev];
    amrex::MultiFab &u_lev = velocity[lev];
    amrex::MultiFab &force_lev = force[lev];

    force_lev.setVal(0.0, nghost);
    for (int i = 0; i < particle_num; i++)
    {
        particles[i]->InterpForce(lev, rho_lev, u_lev, force_lev);
        // particles[i]->InterpForceWallModel(lev, rho_lev, u_lev, force_lev);
    }
}

void AmrCoreLBM::SumForce(int lev)
{
    MultiFab *mf_pointer = &force[lev];

    mf_pointer->SumBoundary(Geom(lev).periodicity());
}

void AmrCoreLBM::ComputeParticle(int lev)
{
    // amrex::AllPrint()<<"ComputeParticle on " << lev <<std::endl;
    CommunicateLevel(lev);
    ComputeMacroLevel(lev);
    InterpForce(lev);
    SumForce(lev);
}

void AmrCoreLBM::ReduceFxy(int lev, int step)
{
    for (int i = 0; i < particle_num; i++)
    {
        particles[i]->SaveFxy(lev, step);
    }
}

void AmrCoreLBM::SaveParticleVelocity(int lev, int step)
{
    for (int i = 0; i < particle_num; i++)
    {
        particles[i]->SaveVelocity(lev, step);
    }
}

void AmrCoreLBM::SaveParticlePosition(int lev, int step)
{
    for (int i = 0; i < particle_num; i++)
    {
        particles[i]->SavePosition(lev, step);
    }
}

void AmrCoreLBM::SaveParticleDistance(int lev, int step)
{
    if (ParallelDescriptor::MyProc() == ParallelDescriptor::IOProcessorNumber())
    {
        std::string filename = "dist.dat";
        std::ofstream file(filename, std::ios::app);

        if (!file.is_open())
        {
            std::cerr << "Cannot open the file: " << filename << std::endl;
            std::exit(1); // 错误退出
        }

        amrex::Real lx = points[0][0] - points[1][0];
        amrex::Real ly = points[0][1] - points[1][1];
        amrex::Real lz = points[0][2] - points[1][2];

        amrex::Real dist = std::sqrt(lx * lx + ly * ly + lz * lz) / D - 1.0;

        file << step << "\t" << dist << "\n";
        file.close();
    }
}

void AmrCoreLBM::PrintParticleParm()
{
    particles[0]->PrintParticleParm();
}

void AmrCoreLBM::RedistributeParticle()
{
    // amrex::AllPrint()<< "RedistributeParticle" << std::endl;
    for (int i = 0; i < particle_num; i++)
    {
        particles[i]->Redistribute();
    }
}

void AmrCoreLBM::InitCpPoint(int lev)
{
    for (int i = 0; i < particle_num; i++)
    {
        particlesCp[i] = std::make_unique<AuxiliaryPointContainer>(this, points[i], i);
        particlesCp[i]->InitCpPoint(lev);
    }
}

void AmrCoreLBM::ComputeCp(int lev, int step)
{
    CommunicateLevel(lev);
    ComputeMacroLevel(lev);

    amrex::MultiFab &rho_lev = density[lev];

    for (int i = 0; i < particle_num; i++)
    {
        particlesCp[i]->InterpCp(lev, rho_lev);
        particlesCp[i]->WriteCp(step);
    }
}

void AmrCoreLBM::LubForceParticle(int lev, amrex::Real cur_time)
{
    // 在这里遍历，然后传入两个颗粒
    for (int p1 = 0; p1 < particle_num; p1++)
    {
        for (int p2 = p1 + 1; p2 < particle_num; p2++)
        {
            particles[p1]->CollideParticle(particles[p2]);
        }
    }

    for (int p1 = 0; p1 < particle_num; p1++)
    {
        particles[p1]->CollideWall();
    }
}

void AmrCoreLBM::MoveParticle(int lev, amrex::Real cur_time)
{
    // amrex::AllPrint()<<"MoveParticle on " << lev <<std::endl;
    for (int i = 0; i < particle_num; i++)
    {
        particles[i]->MoveParticle(lev, cur_time);
        particles[i]->Redistribute();
    }
}

//********************************************************************//
//                     Pure virtual function                          //
//********************************************************************//
void AmrCoreLBM::MakeNewLevelFromCoarse(int lev, amrex::Real time, const amrex::BoxArray &ba,
                                        const amrex::DistributionMapping &dm) // 暂时用不到
{
    // amrex::AllPrint()<<"MakeNewLevelFromCoarse on " << lev <<std::endl;

    if (lev == 0)
    {
        amrex::Abort("Cannot construct level 0 from a coarser level.");
    }

    amrex::MultiFab &u_lev = velocity.at(lev);
    amrex::MultiFab &rho_lev = density.at(lev);
    amrex::MultiFab &vort_lev = vorticity.at(lev);
    amrex::MultiFab &force_lev = force.at(lev);
    amrex::MultiFab &shear_lev = shear.at(lev);
    amrex::MultiFab &f_new_lev = f_new.at(lev);
    amrex::MultiFab &f_old_lev = f_old.at(lev);

    u_lev.define(ba, dm, AMREX_SPACEDIM, nghost);
    rho_lev.define(ba, dm, 1, nghost);
    vort_lev.define(ba, dm, 2, nghost); // 改成两个，分别存vort和q
    force_lev.define(ba, dm, AMREX_SPACEDIM, nghost);
    shear_lev.define(ba, dm, 1, nghost);
    f_new_lev.define(ba, dm, Q, nghost);
    f_old_lev.define(ba, dm, Q, nghost);

    FillCoarsePatch(lev, time, f_old_lev);
}
void AmrCoreLBM::RemakeLevel(int lev, amrex::Real time, const amrex::BoxArray &ba,
                             const amrex::DistributionMapping &dm)
{
    // amrex::AllPrint()<<"ReMakeLevel on " << lev <<std::endl;

    amrex::MultiFab new_state(ba, dm, Q, nghost);
    amrex::MultiFab old_state(ba, dm, Q, nghost);
    amrex::MultiFab u_new(ba, dm, AMREX_SPACEDIM, nghost); // 什么用,要初始化吗
    amrex::MultiFab rho_new(ba, dm, 1, nghost);
    amrex::MultiFab vort_new(ba, dm, 2, nghost);
    amrex::MultiFab force_new(ba, dm, AMREX_SPACEDIM, nghost);
    amrex::MultiFab shear_new(ba, dm, 1, nghost);

    FillDdfPatch(lev, time, old_state);
    // FillPatch(lev, time, old_state);

    std::swap(new_state, f_new[lev]);
    std::swap(old_state, f_old[lev]);

    std::swap(u_new, velocity[lev]);
    std::swap(rho_new, density[lev]);
    std::swap(vort_new, vorticity[lev]);
    std::swap(force_new, force[lev]);
    std::swap(shear_new, shear[lev]);

    force[lev].setVal(0.0, nghost);
    shear[lev].setVal(0.0, nghost);
    vorticity[lev].setVal(0.0, nghost);
}
void AmrCoreLBM::ClearLevel(int lev)
{
    // amrex::AllPrint()<<"ClearLevel on " << lev <<std::endl;

    f_old[lev].clear();
    f_new[lev].clear();
    velocity[lev].clear();
    vorticity[lev].clear();
    density[lev].clear();
    shear[lev].clear();
    force[lev].clear();
}
void AmrCoreLBM::MakeNewLevelFromScratch(int lev, amrex::Real time, const amrex::BoxArray &ba,
                                         const amrex::DistributionMapping &dm)
{
    amrex::MultiFab &u_lev = velocity.at(lev);
    amrex::MultiFab &rho_lev = density.at(lev);
    amrex::MultiFab &vort_lev = vorticity.at(lev);
    amrex::MultiFab &force_lev = force.at(lev);
    amrex::MultiFab &shear_lev = shear.at(lev);
    amrex::MultiFab &f_new_lev = f_new.at(lev);
    amrex::MultiFab &f_old_lev = f_old.at(lev);

    u_lev.define(ba, dm, AMREX_SPACEDIM, nghost);
    rho_lev.define(ba, dm, 1, nghost);
    vort_lev.define(ba, dm, 2, nghost);
    force_lev.define(ba, dm, AMREX_SPACEDIM, nghost);
    shear_lev.define(ba, dm, 1, nghost);
    f_new_lev.define(ba, dm, Q, nghost);
    f_old_lev.define(ba, dm, Q, nghost);

    force_lev.setVal(0.0, nghost); // 在这里归零会不会好一点
    shear_lev.setVal(0.0, nghost);
    vort_lev.setVal(0.0, nghost);

    amrex::Real dx = Geom(lev).CellSizeArray()[0];
    int right = Geom(lev).Domain().length(0) - 1;
    int back = Geom(lev).Domain().length(1) - 1;
    int up = Geom(lev).Domain().length(2) - 1;
    amrex::IntVect hi{right, back, up};

    for (MFIter mfi(f_old_lev, TilingIfNotGPU()); mfi.isValid(); ++mfi)
    {
        const Box &bx = mfi.growntilebox(nghost);
        Array4<Real> const &fold = f_old_lev.array(mfi);
        Array4<Real> const &fnew = f_new_lev.array(mfi);

        amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k)
                           {
            // init_fluid(i, j, k, fold, fnew);
            init_fluid_channel(i, j, k, fold, fnew, hi, dx); });
    }
}

/*************************************第3版*ErrorEst***************************************/
void AmrCoreLBM::ErrorEst(int lev, amrex::TagBoxArray &tags, amrex::Real time, int ngrow)
{
    // amrex::AllPrint()<<"ErrorEst on " << lev <<std::endl;

    if (lev >= err.size())
    {
        return;
    }

    ComputeMacroLevel(lev);
    FillMacroGhostLevel(lev, time);
    ComputeVorticityLevel(lev); // 计算vort比较慢

    const int tagval = TagBox::SET;
    const int clearval = TagBox::CLEAR;

    const MultiFab &f_old_lev = f_old[lev];
    const MultiFab &vort_lev = vorticity[lev];

    amrex::IntVect lo1 = static_lo[lev];
    amrex::IntVect hi1 = static_hi[lev];

    amrex::IntVect lo2 = static_lo[lev + max_ref_level + 1];
    amrex::IntVect hi2 = static_hi[lev + max_ref_level + 1];

    const auto geomdata = geom[lev].data();
    amrex::Gpu::DeviceVector<amrex::RealVect> points_d = convertToDeviceVector(points);

    for (MFIter mfi(f_old_lev, TilingIfNotGPU()); mfi.isValid(); ++mfi)
    {
        const Box &bx = mfi.growntilebox(0);
        const auto vort = vort_lev.array(mfi);
        const auto tagfab = tags.array(mfi);

        const IntVect &lo = bx.smallEnd();
        const IntVect &hi = bx.bigEnd();

        Real err_value = err[lev];
        RealVect *points_p = points_d.data();
        const int points_num = particle_num;

        amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k)
                           {
            // state_error_2(i, j, k, tagfab, vort, err_value, tagval, clearval, lev, geomdata, lo2, hi2, pos);
            // state_error_3(i, j, k, tagfab, vort, err_value, tagval, clearval, lev, geomdata, lo2, hi2, points_p, points_num);
            // state_error_4(i, j, k, tagfab, vort, err_value, tagval, clearval, lev, geomdata, lo2, hi2, points_p, points_num);
            state_error_5(i, j, k, tagfab, vort, err_value, tagval, clearval, lev, geomdata, lo2, hi2, points_p, points_num); });
    }
}