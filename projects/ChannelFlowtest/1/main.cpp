#include <iostream>

#include <AMReX.H>
#include <AMReX_BLProfiler.H>
#include <AMReX_ParallelDescriptor.H>

#include "AmrCoreLBM.H"

using namespace amrex;

void RohdeCycle(int lev, amrex::Real cur_time, AmrCoreLBM& lid); // 好像更适配cumulant_opt
void JaberCycle(int lev, amrex::Real cur_time, AmrCoreLBM& lid); // 更适配cumulant
void RohdeCycleMultiParticle(int lev, amrex::Real cur_time, AmrCoreLBM& lid);
void JaberCycleMultiParticle(int lev, amrex::Real cur_time, AmrCoreLBM& lid);

int main(int argc, char* argv[]) {
    amrex::Initialize(argc, argv);

    {
        const Real start_time = amrex::second();

        int max_step, regrid_int, plot_int, begin_plot;
        amrex::Real stop_time;

        {
            amrex::ParmParse pp;
            pp.query("max_step", max_step);
            pp.query("stop_time", stop_time);
        }
        {
            amrex::ParmParse pp("amr");
            pp.query("regrid_int", regrid_int);
            pp.query("plot_int", plot_int);
            pp.query("begin_plot", begin_plot);
        }

        // ===================== 读取 inputs 中的 blocking_factor 覆盖默认值 =====================
        IntVect bf_components(AMREX_D_DECL(8, 8, 8));
        {
            ParmParse pp("amr");

            Vector<int> bf_all;
            if (pp.countval("blocking_factor") > 0) {
                pp.getarr("blocking_factor", bf_all);
                if (!bf_all.empty()) {
                    bf_components[0] = bf_all[0];
                    if (bf_all.size() > 1)
                        bf_components[1] = bf_all[1];
                    if (bf_all.size() > 2)
                        bf_components[2] = bf_all[2];
                }
            }

            Vector<int> tmp;
            if (pp.countval("blocking_factor_x") > 0) {
                pp.getarr("blocking_factor_x", tmp);
                if (!tmp.empty())
                    bf_components[0] = tmp[0];
            }
            if (pp.countval("blocking_factor_y") > 0) {
                tmp.clear();
                pp.getarr("blocking_factor_y", tmp);
                if (!tmp.empty())
                    bf_components[1] = tmp[0];
            }
            if (pp.countval("blocking_factor_z") > 0) {
                tmp.clear();
                pp.getarr("blocking_factor_z", tmp);
                if (!tmp.empty())
                    bf_components[2] = tmp[0];
            }
        }

        // ==================================================================================

        amrex::Geometry geom(
            amrex::Box({AMREX_D_DECL(0, 0, 0)}, {AMREX_D_DECL(NX - 1, NY - 1, NZ - 1)}),
            amrex::RealBox({AMREX_D_DECL(0., 0., 0.)}, {AMREX_D_DECL(nx, ny, nz)}),
            amrex::CoordSys::cartesian,
            {AMREX_D_DECL(1, 0, 1)});
        amrex::AmrInfo info{
            1,             // verbose
            max_ref_level, // max_level
            amrex::Vector<amrex::IntVect>{(size_t)max_ref_level + 1, {AMREX_D_DECL(2, 2, 2)}},
            amrex::Vector<amrex::IntVect>{(size_t)max_ref_level + 1, bf_components},
            amrex::Vector<amrex::IntVect>{(size_t)max_ref_level + 1, {AMREX_D_DECL(128, 128, 128)}}};

        amrex::Real cur_time = 0.0;

        AmrCoreLBM lid(geom, info);
        lid.InitMesh(cur_time);
        lid.PrintMeshInfo();
        lid.PrintLbmParm();
        lid.InitParticle(max_ref_level);
        lid.InitCpPoint(max_ref_level);
        lid.PrintParticleParm();

        for (int step = 1; step <= max_step && cur_time < stop_time; step++) {
            amrex::Print() << "STEP " << step << "starts ..." << std::endl;

            // if(step >= 0 && step % regrid_int == 0)
            // {
            //     lid.AverageDownValid();
            //     lid.FindCentre();
            //     lid.RefineMesh(cur_time);
            //     lid.RedistributeParticle();
            // }

            /*---------------用于计算静止圆球绕流----------------------------------*/
            // RohdeCycle(0, cur_time, lid);
            JaberCycle(0, cur_time, lid);
            lid.ReduceFxy(max_ref_level, step);
            /*--------------------------------------------------------------------*/

            /*---------------用于计算多颗粒自由运动----------------------------------*/
            // RohdeCycleMultiParticle(0, cur_time, lid);
            // JaberCycleMultiParticle(0, cur_time, lid);

            // lid.SaveParticlePosition(0, step);
            // lid.SaveParticleVelocity(0, step);
            // lid.SaveParticleDistance(0, step);
            /*--------------------------------------------------------------------*/

            cur_time += dt_0;

            // if(step >= 98000 && step <= 100000 && step % 100 == 0)
            // {
            //     lid.ComputeCp(max_ref_level, step);
            // }

            if (step >= begin_plot && step % plot_int == 0) {
                lid.PrintMeshInfo();
                lid.ComputeMacro();
                lid.ComputeVorticity(cur_time);
                lid.WriteVelocityFile(step, cur_time);
                // lid.WriteDensityFile(step, cur_time);
                // lid.ComputeCp(max_ref_level, step);
                // lid.WriteMultiParticleFile(step, cur_time);
                // lid.WriteVelocityFile(step, cur_time, max_ref_level);
                // lid.WriteParticleFile(step, cur_time);
                // lid.WriteVorticityFile(step, cur_time);
                // lid.WriteVelocityFileWithParticle(step, cur_time);
            }

            // 每步调用，函数内部根据 step 与 FT 判断是否真正采样
            lid.RecordMeanVelocityProfile(step);
        }

        amrex::Real end_total = amrex::second() - start_time;
        if (lid.Verbose()) {
            ParallelDescriptor::ReduceRealMax(end_total, ParallelDescriptor::IOProcessorNumber());
            amrex::Print() << "\nTotal Time: " << end_total << '\n';
        }
    }

    amrex::Finalize();

    return 0;
}

/*---------------用于计算静止圆球绕流----------------------------------*/
void RohdeCycle(int lev, amrex::Real cur_time, AmrCoreLBM& lid) {
    amrex::Real dt = lid.Geom(lev).CellSizeArray()[0];

    if (lev == max_ref_level) {
        lid.ComputeParticle(lev);
    }

    lid.Boundary(lev);
    lid.Collide(lev, 0);

    if (lev < max_ref_level) {
        RohdeCycle(lev + 1, cur_time, lid);
    }

    if (lev > coarsest_level) {
        lid.FillGhostLevel(lev, cur_time, 0);
    }

    lid.CommunicateLevel(lev);
    lid.Stream(lev, 2);
    lid.SwapLevel(lev, 2);

    if (lev < max_ref_level) {
        lid.AverageDownGhostLevel(lev, 0);
    }

    if (lev == coarsest_level) {
        return;
    }

    cur_time += dt;

    if (lev == max_ref_level) {
        lid.ComputeParticle(lev);
    }

    lid.Boundary(lev);
    lid.Collide(lev, 0);

    if (lev < max_ref_level) {
        RohdeCycle(lev + 1, cur_time, lid);
    }

    lid.CommunicateLevel(lev);
    lid.Stream(lev, 2);
    lid.SwapLevel(lev, 2);

    if (lev < max_ref_level) {
        lid.AverageDownGhostLevel(lev, 0);
    }
}

/*---------------用于计算静止圆球绕流----------------------------------*/
void JaberCycle(int lev, amrex::Real cur_time, AmrCoreLBM& lid) {
    amrex::Real dt = lid.Geom(lev).CellSizeArray()[0];

    if (lev < max_ref_level) {
        lid.FillGhostLevel(lev + 1, cur_time, 1);
    }

    // if(lev == max_ref_level)
    // {
    //     lid.ComputeParticle(lev);
    //     lid.FillForceGhostLevel(lev, cur_time);//加一个力的填充ghost就好了
    // }

    lid.Boundary(lev);
    lid.Collide(lev, 4);
    lid.CommunicateLevel(lev);
    lid.Stream(lev, 4);
    lid.SwapLevel(lev, 4);

    if (lev < max_ref_level) {
        JaberCycle(lev + 1, cur_time, lid);
        lid.AverageDownGhostLevel(lev, 1);
    }

    if (lev == coarsest_level) {
        return;
    }

    cur_time += dt;

    if (lev < max_ref_level) {
        lid.FillGhostLevel(lev + 1, cur_time, 1);
    }

    // if(lev == max_ref_level)
    // {
    //     lid.ComputeParticle(lev);
    //     lid.FillForceGhostLevel(lev, cur_time);//加一个力的填充ghost就好了
    // }

    lid.Boundary(lev);
    lid.Collide(lev, 4);
    lid.CommunicateLevel(lev);
    lid.Stream(lev, 4);
    lid.SwapLevel(lev, 4);

    if (lev < max_ref_level) {
        JaberCycle(lev + 1, cur_time, lid);
        lid.AverageDownGhostLevel(lev, 1);
    }
}

/*---------------用于计算多颗粒自由运动----------------------------------*/
void RohdeCycleMultiParticle(int lev, amrex::Real cur_time, AmrCoreLBM& lid) {
    amrex::Real dt = lid.Geom(lev).CellSizeArray()[0];

    if (lev == max_ref_level) {
        lid.ComputeParticle(lev);
        // lid.FillForceGhostLevel(lev, cur_time);
        lid.LubForceParticle(lev, cur_time);
        lid.MoveParticle(lev, cur_time);
    }

    lid.Boundary(lev);
    lid.Collide(lev, 0);

    if (lev < max_ref_level) {
        RohdeCycleMultiParticle(lev + 1, cur_time, lid);
    }

    if (lev > coarsest_level) {
        lid.FillGhostLevel(lev, cur_time, 0);
    }

    lid.CommunicateLevel(lev);
    lid.Stream(lev, 2);
    lid.SwapLevel(lev, 2);

    if (lev < max_ref_level) {
        lid.AverageDownGhostLevel(lev, 0);
    }

    if (lev == coarsest_level) {
        return;
    }

    cur_time += dt;

    if (lev == max_ref_level) {
        lid.ComputeParticle(lev);
        // lid.FillForceGhostLevel(lev, cur_time);
        lid.LubForceParticle(lev, cur_time);
        lid.MoveParticle(lev, cur_time);
    }

    lid.Boundary(lev);
    lid.Collide(lev, 0);

    if (lev < max_ref_level) {
        RohdeCycleMultiParticle(lev + 1, cur_time, lid);
    }

    lid.CommunicateLevel(lev);
    lid.Stream(lev, 2);
    lid.SwapLevel(lev, 2);

    if (lev < max_ref_level) {
        lid.AverageDownGhostLevel(lev, 0);
    }
}

/*---------------用于计算多颗粒自由运动----------------------------------*/
void JaberCycleMultiParticle(int lev, amrex::Real cur_time, AmrCoreLBM& lid) {
    amrex::Real dt = lid.Geom(lev).CellSizeArray()[0];

    if (lev < max_ref_level) {
        lid.FillGhostLevel(lev + 1, cur_time, 1);
    }

    if (lev == max_ref_level) {
        lid.ComputeParticle(lev);
        lid.FillForceGhostLevel(lev, cur_time); // 加一个力的填充ghost就好了
        lid.LubForceParticle(lev, cur_time);
        lid.MoveParticle(lev, cur_time);
    }

    lid.Boundary(lev);
    lid.Collide(lev, 4);
    lid.CommunicateLevel(lev);
    lid.Stream(lev, 4);
    lid.SwapLevel(lev, 4);

    if (lev < max_ref_level) {
        JaberCycleMultiParticle(lev + 1, cur_time, lid);
        lid.AverageDownGhostLevel(lev, 1);
    }

    if (lev == coarsest_level) {
        return;
    }

    cur_time += dt;

    if (lev < max_ref_level) {
        lid.FillGhostLevel(lev + 1, cur_time, 1);
    }

    if (lev == max_ref_level) {
        lid.ComputeParticle(lev);
        lid.FillForceGhostLevel(lev, cur_time); // 加一个力的填充ghost就好了
        lid.LubForceParticle(lev, cur_time);
        lid.MoveParticle(lev, cur_time);
    }

    lid.Boundary(lev);
    lid.Collide(lev, 4);
    lid.CommunicateLevel(lev);
    lid.Stream(lev, 4);
    lid.SwapLevel(lev, 4);

    if (lev < max_ref_level) {
        JaberCycleMultiParticle(lev + 1, cur_time, lid);
        lid.AverageDownGhostLevel(lev, 1);
    }
}