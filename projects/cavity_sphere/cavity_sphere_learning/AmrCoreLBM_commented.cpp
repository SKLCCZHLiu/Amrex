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

// Linter 错误提示：找不到 "curand.h"。
// 这是一个CUDA相关的头文件，用于GPU上的随机数生成。
// 通常在启用GPU编译时，AMReX的头文件会间接地包含它。
// 这通常是环境配置问题（比如 include path 设置不正确），而不是代码本身的错误。

using namespace amrex;

/**
 * @brief 一个辅助模板函数，用于将主机端的 amrex::Vector 转换为GPU设备端的 amrex::Gpu::DeviceVector。
 *        这在需要在GPU上访问主机端定义的数组（如粒子位置）时非常有用。
 * @tparam T 向量中存储的数据类型。
 * @param v 主机端的 amrex::Vector。
 * @return 转换后的 amrex::Gpu::DeviceVector。
 */
template <class T>
amrex::Gpu::DeviceVector<T>
convertToDeviceVector(amrex::Vector<T> v)
{
    int ncomp = v.size();
    amrex::Gpu::DeviceVector<T> v_d(ncomp);
#ifdef AMREX_USE_GPU
    // 如果使用GPU，需要执行从主机到设备的内存拷贝。
    amrex::Gpu::htod_memcpy(v_d.data(), v.data(), sizeof(T) * ncomp);
#else
    // 如果在CPU上运行，直接使用标准库的 memcpy 即可。
    std::memcpy(v_d.data(), v.data(), sizeof(T) * ncomp);
#endif
    return v_d;
}

//********************************************************************//
//                           构造函数 (Constructor)                      //
//********************************************************************//
/**
 * @brief AmrCoreLBM 类的构造函数。
 *        负责初始化整个模拟环境，包括参数读取、数据结构分配和物理参数设置。
 * @param level_0_geom 最粗层级的几何信息。
 * @param amr_info 自适应网格相关的配置信息。
 */
AmrCoreLBM::AmrCoreLBM(amrex::Geometry const &level_0_geom, amrex::AmrInfo const &amr_info) : AmrCore(level_0_geom, amr_info)
{
    // 从输入文件中读取参数
    ReadParameters();

    int nlevs_max = max_level + 1;

    // 为每个可能的网格层级分配存储空间
    f_new.resize(nlevs_max); // 存储新时间步的分布函数
    f_old.resize(nlevs_max); // 存储旧时间步的分布函数

    velocity.resize(nlevs_max);  // 宏观速度
    vorticity.resize(nlevs_max); // 涡量
    shear.resize(nlevs_max);     // 剪切率 (用于湍流模型)
    density.resize(nlevs_max);   // 宏观密度
    force.resize(nlevs_max);     // 体积力 (用于浸入边界法)

    // LBM的松弛时间 tau 对每个层级都不同，因为它依赖于时间步长dt，而不同层级的dt不同。
    // 这里根据最粗层级的 tau_0 进行递推计算。
    tau.resize(nlevs_max);
    tau[0] = tau_0;
    for (int lev = 1; lev <= max_level; ++lev)
    {
        // 这是一个保证在不同层级间物理粘性一致的递推关系。
        tau[lev] = 2 * (tau[lev - 1] - 0.5) + 0.5;
    }

    // 设置物理边界条件类型。这里所有边界都使用一阶外插（First Order Extrapolation）。
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

    // 初始化用于静态加密的区域边界
    static_lo.resize(2 * nlevs_max);
    static_hi.resize(2 * nlevs_max);

    // 初始化颗粒（粒子）
    particles.resize(particle_num);
    points.resize(particle_num);
    // 这里可以从文件或通过算法生成初始粒子位置
    points[0] = {X, Y, Z};

    // 初始化用于计算表面压力系数Cp的辅助粒子容器
    particlesCp.resize(particle_num);

    // 为每个层级定义静态加密区域。
    // 这段被注释掉的代码展示了如何根据物理尺寸（如颗粒直径D）和层级分辨率dx来定义不同层级的固定加密框。
    for (int lev = 0; lev <= max_level; lev++)
    {
        amrex::Real dx = Geom(lev).CellSizeArray()[0];

        // 示例：在离物理边界8个单元格内进行加密
        for (int idim = 0; idim < AMREX_SPACEDIM; idim++)
        {
            static_lo[lev][idim] = 0 + (8);
            static_hi[lev][idim] = Geom(lev).Domain().length(idim) - (8);
        }
    }
}

AmrCoreLBM::AmrCoreLBM()
{
}
AmrCoreLBM::~AmrCoreLBM() = default;

//********************************************************************//
//                        辅助函数 (Helper Functions)                    //
//********************************************************************//
/**
 * @brief 打印网格相关的信息，如每个层级的网格数量、总单元数等。
 *        只在IO主进程上执行，避免重复打印。
 */
void AmrCoreLBM::PrintMeshInfo()
{
    if (ParallelDescriptor::IOProcessor())
    {
        amrex::Print() << "╔════════════════════════════════════════════════════════╗" << std::endl;
        amrex::Print() << "║               Mesh Information                         ║" << std::endl;
        amrex::Print() << "╚════════════════════════════════════════════════════════╝" << std::endl;
        printGridSummary(amrex::OutStream(), 0, finest_level);
        // ... 打印其他网格参数 ...
        amrex::Print() << "╚════════════════════════════════════════════════════════╝" << std::endl;
        amrex::Print() << std::endl;
    }
}

/**
 * @brief 打印LBM相关的物理和数值参数。
 */
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
    amrex::Print() << std::setw(15) << std::left << "  U0     =" << std::setw(10) << std::right << U0 << std::endl;

    for(int lev = 0; lev <= finest_level; lev++)
    {
        amrex::Print() << std::setw(15) << std::left << "  tau    ="<< std::setw(10) << std::right << tau[lev] << std::endl;
    }

    amrex::Print() << "╚════════════════════════════════════════════════════════╝" << std::endl;
    amrex::Print() << std::endl;
}

/**
 * @brief 使用 AMReX 的 ParmParse 工具从输入文件中读取参数。
 *        这是一个非常灵活的机制，允许用户在运行时配置模拟。
 */
void AmrCoreLBM::ReadParameters()
{
    {
        // "amr" 是输入文件中的一个前缀，例如 amr.plot_file = plotfiles
        ParmParse pp("amr");
        pp.query("plot_file", plot_file);
        pp.query("grid_eff", grid_eff);

        // 读取每个层级的 n_error_buf (加密缓冲区域大小)
        int cnt = pp.countval("n_error_buf");
        if (cnt > 0)
        {
            Vector<int> neb;
            pp.getarr("n_error_buf", neb);
            // ... (代码处理读取的数组并赋给所有层级)
        }
        // ... (类似地读取 n_error_buf_x, y, z, max_grid_size, blocking_factor等)
    }

    {
        // 读取LBM特定的参数
        ParmParse pp("lbm");
        int n = pp.countval("err"); // 加密阈值
        if (n > 0)
        {
            pp.getarr("err", err, 0, n);
        }
    }
}

/**
 * @brief 将每个层级的宏观速度场数据写入多层级的 plotfile。
 *        这是AMReX标准的用于可视化的输出格式。
 * @param step 当前的时间步数，用于命名文件。
 * @param time 当前的物理时间。
 */
void AmrCoreLBM::WriteVelocityFile(const int step, const amrex::Real time)
{
    const std::string &plotfilename = amrex::Concatenate(plot_file, step, 6);

    // 创建一个包含所有层级速度MultiFab指针的Vector
    amrex::Vector<const amrex::MultiFab *> mf;
    for (int i = 0; i <= finest_level; ++i)
    {
        mf.push_back(&velocity[i]);
    }

    // 定义变量名，用于在可视化软件中识别
    amrex::Vector<std::string> varnames = {"ux", "uy", "uz"};

    // 调用AMReX的工具函数写入文件
    amrex::WriteMultiLevelPlotfile(plotfilename, finest_level + 1, mf, varnames,
                                   Geom(), time, Vector<int>(finest_level + 1, step), refRatio());
}

// ... (其他Write...File函数与WriteVelocityFile类似，只是写入不同的物理量)

void AmrCoreLBM::WriteDensityFile(const int step, const amrex::Real time) { /* ... */ }
void AmrCoreLBM::WriteVelocityFile(const int step, const amrex::Real time, const int lev) { /* ... */ }
void AmrCoreLBM::WriteVorticityFile(const int step, const amrex::Real time) { /* ... */ }
void AmrCoreLBM::WriteParticleFile(const int step, const amrex::Real time) { /* ... */ }
void AmrCoreLBM::WriteMultiParticleFile(const int step, const amrex::Real time) { /* ... */ }

//********************************************************************//
//                        网格操作函数 (Mesh Functions)                  //
//********************************************************************//
/**
 * @brief 初始化网格。对于AMR模拟，这通常意味着从零开始创建第0层。
 */
void AmrCoreLBM::InitMesh(amrex::Real cur_time)
{
    InitFromScratch(cur_time);
}

/**
 * @brief AMR回调函数：用粗网格数据填充细网格的有效区域（valid region）。
 *        当一个新层级被创建时，需要从更粗的层级插值来获得其初始状态。
 * @param lev 要被填充的细网格层级。
 * @param time 当前物理时间。
 * @param mf 要被填充的MultiFab。
 */
void AmrCoreLBM::FillCoarsePatch(int lev, amrex::Real time, amrex::MultiFab &mf)
{
    // ... (这个函数体在当前代码中似乎未被使用，注释为"根本没有用到")
    // 它展示了如何使用 amrex::InterpFromCoarseLevel 进行手动插值。
}

/**
 * @brief AMR核心函数：填充一个MultiFab的幽灵层（ghost cells）。
 *        这是AMReX中数据通信和边界处理的核心。
 * @param lev 要操作的层级。
 * @param time 当前物理时间。
 * @param mf 要填充幽灵层的MultiFab。
 */
void AmrCoreLBM::FillPatch(int lev, amrex::Real time, amrex::MultiFab &mf)
{
    Interpolater *mapper = &cell_cons_interp; // 指定插值算子类型

    if (lev == 0)
    {
        // 最粗层级没有更粗的层级，所以它只在同层级内部填充幽灵层（例如，从一个Box填充另一个相邻Box的幽灵层）。
        // 同时，物理边界条件也在这里被施加。
        // FillPatchSingleLevel处理单层级内的填充。
        // ...
    }
    else
    {
        // 细网格（lev > 0）需要处理两种情况：
        // 1. 同层级Box之间的边界。
        // 2. 和粗网格（lev-1）的交界处（Coarse-Fine Interface）。
        // FillPatchTwoLevels优雅地处理了这两种情况。它会优先使用同层级的细网格数据填充，
        // 如果邻居在粗网格上，则会自动调用mapper从粗网格插值。
        // ...
    }
}

/**
 * @brief 一个自定义的FillPatch版本，专门用于填充分布函数(ddf)。
 *        它在插值之前增加了一个'scale'操作。
 *        这对于某些需要在层级间传递非平衡态信息的AMR-LBM算法是必需的。
 */
void AmrCoreLBM::FillDdfPatch(int lev, amrex::Real time, amrex::MultiFab &mf)
{
    // ...
    // 在调用标准的 FillPatchTwoLevels 之前，对粗网格数据 f_old[lev-1] 进行 in-place 的缩放操作，
    // 结果存储在临时 MultiFab f_new[lev-1] 中，然后用这个缩放后的数据去插值。
    amrex::Real scale = tau[lev] / tau[lev - 1] / 2.0;
    // ...
}

/**
 * @brief 与FillPatch类似，但用于填充宏观物理量（如速度）。
 */
void AmrCoreLBM::FillMacroPatch(int lev, amrex::Real time, amrex::MultiFab &mf)
{
    // ... (逻辑与 FillPatch 相同，只是操作的数据是 velocity 而不是 f_old)
}

/**
 * @brief 一个简单的包装函数，调用AMR核心的regrid函数来重新生成网格。
 */
void AmrCoreLBM::RefineMesh(amrex::Real cur_time)
{
    regrid(0, cur_time);
}

/**
 * @brief 在重绘网格之前，更新所有粒子的中心位置。
 */
void AmrCoreLBM::FindCentre()
{
    for (int p_num = 0; p_num < particle_num; p_num++)
    {
        points[p_num] = particles[p_num]->ReturnCentre();
    }
}

//********************************************************************//
//                        LBM核心算法函数                             //
//********************************************************************//

/**
 * @brief 从微观的分布函数 f_old 计算宏观量（密度和速度）。
 * @param lev 要计算的层级。
 */
void AmrCoreLBM::ComputeMacroLevel(int lev)
{
    // ... (通过MFIter遍历网格)
    // 在每个单元上调用 `compute_macro` 内核函数。
}

void AmrCoreLBM::ComputeMacro()
{
    for (int lev = 0; lev <= finest_level; lev++)
    {
        ComputeMacroLevel(lev);
    }
}

/**
 * @brief 计算涡量。这是一个后处理步骤。
 *        需要先填充速度场的幽灵层，因为计算涡量需要用到中心差分。
 */
void AmrCoreLBM::ComputeVorticity(amrex::Real cur_time)
{
    for (int lev = 0; lev <= finest_level; lev++)
    {
        FillMacroGhostLevel(lev, cur_time); // 填充速度场的幽灵层
        ComputeVorticityLevel(lev);         // 计算涡量
    }
}
void AmrCoreLBM::ComputeVorticityLevel(int lev) { /* ... */ }

/**
 * @brief 计算剪切率。用于SGS（亚格子）模型。
 */
void AmrCoreLBM::ComputeShear()
{
    for (int lev = 0; lev <= finest_level; lev++)
    {
        ComputeShearLevel(lev);
    }
}
void AmrCoreLBM::ComputeShearLevel(int lev) { /* ... */ }

/**
 * @brief AMR操作：用细网格（lev+1）数据更新粗网格（lev）的有效区域。
 *        这确保了粗网格上被细网格覆盖的区域的数据与细网格的（更精确的）数据保持一致。
 *        这是一个"Fine-to-Coarse"的同步操作。
 */
void AmrCoreLBM::AverageDownValidLevel(int lev, bool is_scale)
{
    // amrex::average_down 是AMReX提供的标准下采样工具。
    // 它将 lev+1 上的 ref_ratio*ref_ratio*ref_ratio 个单元的数据平均，然后赋值给 lev 上的1个单元。
    // ...
    amrex::MultiFab& fine_mf = f_old[lev+1];
    amrex::MultiFab& crse_mf = f_old[lev];

    //BoxArray 是一个描述网格布局的元数据结构。它定义了在这一层级上，所有网格块（Box）的几何形状和位置。
    //DistributionMap 是一个描述数据分布的元数据结构。它定义了在这一层级上，所有网格块（Box）的数据存储位置。
    //Q 是分布函数的维度。
    //0 表示不使用Ghost层
    MultiFab fine_boundary_data(fine_mf.boxArray(), fine_mf.DistributionMap(), Q, 0);//能不能用f_new减少内存消耗
    //Copy 是AMReX提供的标准拷贝工具。它将 fine_mf 中的数据拷贝到 fine_boundary_data 中。
    MultiFab::Copy(fine_boundary_data, fine_mf, 0, 0, Q, 0);

    if(is_scale)
    {
        amrex::Real scale = 2.0 * tau[lev]/tau[lev+1]; 

        for(MFIter mfi(fine_boundary_data, TilingIfNotGPU()); mfi.isValid(); ++mfi)
        {
            const auto bx = mfi.growntilebox(0); //获取当前网格块的有效区域

            const Array4<Real>& fold = fine_boundary_data.array(mfi); //获取当前网格块的分布函数数据
    
            amrex::ParallelFor(bx, [=]AMREX_GPU_DEVICE(int i, int j, int k)
            {
                average_scale(i, j, k, fold, scale);
            });
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

/**
 * @brief AMR操作：用细网格（lev+1）数据更新粗网格（lev）的幽灵层。
 *        这与 AverageDownValidLevel 类似，但操作的是幽灵层数据。
 *        在某些特定的时间步算法（如JaberCycle）中，需要在两次子循环之间同步幽灵层。
 */
void AmrCoreLBM::AverageDownGhostLevel(int lev, bool is_scale)
{
    amrex::MultiFab &fine_mf = f_old[lev + 1];
    amrex::MultiFab &crse_mf = f_old[lev];

    // 创建一个临时副本，以避免修改原始的细网格数据。这是一个安全的编程实践。
    MultiFab fine_boundary_data(fine_mf.boxArray(), fine_mf.DistributionMap(), Q, 2);
    MultiFab::Copy(fine_boundary_data, fine_mf, 0, 0, Q, 2);

    // 如果需要，对副本进行物理一致性缩放。
    if (is_scale)
    {
        // ...
    }

    // 将处理后的细网格数据平均下采样到粗网格上。
    amrex::average_down(fine_boundary_data, crse_mf, 0, Q, refRatio(lev));
}
void AmrCoreLBM::AverageDownGhost() {}

/**
 * @brief AMR操作：用粗网格（lev-1）数据填充细网格（lev）的幽灵层。
 *        这是一个"Coarse-to-Fine"的插值操作，为细网格的计算提供边界条件。
 */
void AmrCoreLBM::FillGhostLevel(int lev, amrex::Real time, bool is_scale)
{
    amrex::MultiFab &f_old_lev = f_old[lev];

    if (is_scale)
    {
        // 使用带缩放的自定义FillPatch
        FillDdfPatch(lev, time, f_old_lev);
    }
    else
    {
        // 使用标准的FillPatch
        FillPatch(lev, time, f_old_lev);
    }
}

/**
 * @brief 填充宏观速度场的幽灵层。
 */
void AmrCoreLBM::FillMacroGhostLevel(int lev, amrex::Real time) { /* ... */ }

/**
 * @brief 填充体积力场的幽灵层。
 */
void AmrCoreLBM::FillForceGhostLevel(int lev, amrex::Real time) { /* ... */ }

/**
 * @brief 在同层级的不同网格块之间进行通信，填充幽灵层。
 */
void AmrCoreLBM::CommunicateLevel(int lev)
{
    amrex::MultiFab &f_old_lev = f_old[lev];
    f_old_lev.FillBoundary(geom[lev].periodicity());
}

/**
 * @brief 施加物理边界条件。
 */
void AmrCoreLBM::Boundary(int lev)
{
    // ...
    // 调用 `fill_boundary` 内核函数。
}

/**
 * @brief LBM的碰撞步骤。
 */
void AmrCoreLBM::Collide(int lev, int n)
{
    // ...
    // 调用 `collide`, `collide_cumulant` 或 `collide_cumulant_opt2` 内核函数。
}

/**
 * @brief LBM的迁移（流动）步骤。
 */
void AmrCoreLBM::Stream(int lev, int n)
{
    // ...
    // 调用 `stream` 内核函数。
}

/**
 * @brief 交换新旧分布函数，为下一个时间步做准备。
 *        使用 std::swap 是一个高效的操作，它只交换指针，而没有实际的数据拷贝。
 */
void AmrCoreLBM::SwapLevel(int lev, int n)
{
    amrex::MultiFab &f_old_lev = f_old[lev];
    amrex::MultiFab &f_new_lev = f_new[lev];
    std::swap(f_old_lev, f_new_lev);
}

void AmrCoreLBM::InterpScale(int lev, int n) { /* ... */ }
void AmrCoreLBM::AverageScale(int lev, int n) { /* ... */ }

//********************************************************************//
//                      浸入边界法函数 (IBM Functions)                  //
//********************************************************************//
/**
 * @brief 初始化所有拉格朗日粒子（即浸入的边界）。
 */
void AmrCoreLBM::InitParticle(int lev)
{
    for (int i = 0; i < particle_num; i++)
    {
        particles[i] = std::make_unique<LagrangeParticleContainer>(this, points[i], i);
        particles[i]->InitParticle(lev);
    }
}

/**
 * @brief 浸入边界法核心：插值计算力并施加到流场。
 *        1. 将欧拉网格上的速度插值到拉格朗日点。
 *        2. 根据拉格朗日点上的速度和期望速度计算力。
 *        3. 将计算出的力通过delta函数散播（spread）回欧拉网格，作为体积力。
 */
void AmrCoreLBM::InterpForce(int lev)
{
    // ... (在每个粒子上调用其 InterpForce 方法)
}

/**
 * @brief 在并行计算中，对所有处理器上的力进行求和，确保力守恒。
 */
void AmrCoreLBM::SumForce(int lev)
{
    force[lev].SumBoundary(Geom(lev).periodicity());
}

/**
 * @brief 一个包装函数，协调浸入边界法的所有计算步骤。
 */
void AmrCoreLBM::ComputeParticle(int lev)
{
    CommunicateLevel(lev);
    ComputeMacroLevel(lev);
    InterpForce(lev);
    SumForce(lev);
}

// ... (其他与粒子相关的辅助、输出、移动函数)

void AmrCoreLBM::ReduceFxy(int lev, int step) { /* ... */ }
void AmrCoreLBM::SaveParticleVelocity(int lev, int step) { /* ... */ }
void AmrCoreLBM::SaveParticlePosition(int lev, int step) { /* ... */ }
void AmrCoreLBM::SaveParticleDistance(int lev, int step) { /* ... */ }
void AmrCoreLBM::PrintParticleParm() { /* ... */ }
void AmrCoreLBM::RedistributeParticle() { /* ... */ }
void AmrCoreLBM::InitCpPoint(int lev) { /* ... */ }
void AmrCoreLBM::ComputeCp(int lev, int step) { /* ... */ }
void AmrCoreLBM::LubForceParticle(int lev, amrex::Real cur_time) { /* ... */ }
void AmrCoreLBM::MoveParticle(int lev, amrex::Real cur_time) { /* ... */ }

//********************************************************************//
//                必须实现的AMR纯虚函数 (Pure Virtual Functions)         //
//********************************************************************//
/**
 * @brief AMR回调函数：当一个新层级被创建时，如何从粗网格初始化它。
 */
void AmrCoreLBM::MakeNewLevelFromCoarse(int lev, amrex::Real time, const amrex::BoxArray &ba,
                                        const amrex::DistributionMapping &dm)
{
    // 1. 定义新层级上的所有MultiFab（f_new, f_old, velocity等）。
    // ...
    // 2. 调用FillCoarsePatch，从lev-1插值数据来填充新创建的f_old[lev]。
    FillCoarsePatch(lev, time, f_old[lev]);
}

/**
 * @brief AMR回调函数：当一个已存在的层级因为重绘网格而改变其布局时，如何重建它。
 */
void AmrCoreLBM::RemakeLevel(int lev, amrex::Real time, const amrex::BoxArray &ba,
                             const amrex::DistributionMapping &dm)
{
    // 1. 根据新的BoxArray和DistributionMap创建一套新的MultiFab。
    amrex::MultiFab new_state(ba, dm, Q, nghost);
    // ... (其他新的MultiFab)

    // 2. 从旧的MultiFab布局中，通过插值和拷贝，填充新的MultiFab。
    //    这里使用FillDdfPatch来处理数据从旧布局到新布局的转移。
    FillDdfPatch(lev, time, old_state);

    // 3. 使用std::swap高效地用新数据替换掉旧数据。
    std::swap(new_state, f_new[lev]);
    std::swap(old_state, f_old[lev]);
    // ...
}

/**
 * @brief AMR回调函数：当一个层级被移除时，如何清理它的数据。
 */
void AmrCoreLBM::ClearLevel(int lev)
{
    f_old[lev].clear();
    f_new[lev].clear();
    // ... (清理所有与该层级相关的MultiFab)
}

/**
 * @brief AMR回调函数：如何从零开始创建一个层级（主要用于第0层）。
 */
void AmrCoreLBM::MakeNewLevelFromScratch(int lev, amrex::Real time, const amrex::BoxArray &ba,
                                         const amrex::DistributionMapping &dm)
{
    // 1. 定义新层级上的所有MultiFab。
    // ...
    // 2. 为新定义的MultiFab赋初始值。
    force_lev.setVal(0.0, nghost);
    // ...
    // 3. 调用init_fluid内核函数，为f_old和f_new设置初始的平衡态分布函数。
    amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k)
                       { init_fluid(i, j, k, fold, fnew); });
}

/**
 * @brief AMR核心函数：定义网格加密的准则。
 *        AMReX会调用这个函数来决定哪些单元格需要被加密。
 * @param lev 当前层级。
 * @param tags 一个特殊的MultiFab，用于标记需要加密的单元格。
 * @param time 当前物理时间。
 * @param ngrow 幽灵层宽度。
 */
void AmrCoreLBM::ErrorEst(int lev, amrex::TagBoxArray &tags, amrex::Real time, int ngrow)
{
    if (lev >= err.size())
    {
        return; // 如果当前层级超过了我们设定的最大加密层级，则不进行操作。
    }

    // 为了根据物理量来加密，首先需要计算这些物理量。
    ComputeMacroLevel(lev);
    FillMacroGhostLevel(lev, time); // 计算涡量需要邻居信息
    ComputeVorticityLevel(lev);

    const int tagval = TagBox::SET;     // 加密的标记值
    const int clearval = TagBox::CLEAR; // 不加密的标记值

    // ... 获取需要的数据 ...

    // 在GPU上启动并行循环，遍历每个单元格
    amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k)
                       {
                           // state_error_... 是一系列的内核函数，定义在 Kernels.H 中。
                           // 它们实现了具体的加密逻辑，例如：
                           // - 如果某点的涡量大于某个阈值，则标记它。
                           // - 如果某点在某个几何区域内（如靠近粒子表面），则标记它。
                           // ...
                           state_error_3(i, j, k, tagfab, vort, err_value, tagval, clearval, lev, geomdata, lo2, hi2, points_p, points_num); });
}