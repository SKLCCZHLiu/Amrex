#include <iostream>

#include <AMReX.H>
#include <AMReX_BLProfiler.H>
#include <AMReX_ParallelDescriptor.H>


#include "AmrCoreLBM.H"


using namespace amrex;

void RohdeCycle(int lev, amrex::Real cur_time, AmrCoreLBM& lid);
void JaberCycle(int lev, amrex::Real cur_time, AmrCoreLBM& lid);

int main(int argc, char* argv[])
{
    amrex::Initialize(argc, argv);

    {
        const Real start_time = amrex::second();

        int max_step, regrid_int, plot_int, begin_int;
        amrex::Real stop_time;

        {
            amrex::ParmParse pp;
            pp.query("max_step", max_step);
            pp.query("stop_time", stop_time);
        }
        {
            amrex::ParmParse pp("amr");
            pp.query("regrid_int", regrid_int);
            pp.query("plot_int"  , plot_int);
            pp.query("begin_int" , begin_int);         
        }


        amrex::Geometry geom
        (
            amrex::Box({AMREX_D_DECL(0,0,0)}, {AMREX_D_DECL(NX-1, NY-1, NZ-1)}),
            amrex::RealBox({AMREX_D_DECL(0., 0., 0.)}, {AMREX_D_DECL(nx, ny, nz)}),
            amrex::CoordSys::cartesian,
            {AMREX_D_DECL(0, 0, 0)}
        );
        amrex::AmrInfo info
        {
            1, //verbose
            max_ref_level, //max_level
            amrex::Vector<amrex::IntVect>{(size_t) max_ref_level+1, {AMREX_D_DECL(2, 2, 2)}},
            amrex::Vector<amrex::IntVect>{(size_t) max_ref_level+1, {AMREX_D_DECL(32, 32, 32)}},
            amrex::Vector<amrex::IntVect>{(size_t) max_ref_level+1, {AMREX_D_DECL(128, 128, 128)}}            
        };

        amrex::Real cur_time = 0.0;

        AmrCoreLBM lid(geom, info);
        lid.InitMesh(cur_time);
        lid.PrintMeshInfo();
        lid.PrintLbmParm();
        lid.InitParticle(max_ref_level);
        lid.PrintParticleParm();

        for(int step = 1; step <= max_step && cur_time < stop_time; step++)
        {
            amrex::Print() << "STEP " << step << "starts ..." << std::endl;

            if(step >= 0 && step % regrid_int == 0)
            {
                lid.AverageDownValid();
                lid.RefineMesh(cur_time);
                lid.RedistributeParticle();
            }

            // RohdeCycle(0, cur_time, lid);
            JaberCycle(0, cur_time, lid);

            lid.ReduceFxy(max_ref_level, step);

            cur_time += dt_0;

            if(step >= begin_int && step % plot_int == 0)
            {   
                lid.PrintMeshInfo();   
                lid.ComputeMacro();
                lid.WriteVelocityFile(step, cur_time);
            }

        }

        amrex::Real end_total = amrex::second() - start_time;
        if (lid.Verbose())
        {
            ParallelDescriptor::ReduceRealMax(end_total ,ParallelDescriptor::IOProcessorNumber());
            amrex::Print() << "\nTotal Time: " << end_total << '\n';
        }
    }

    amrex::Finalize();
    
    return 0;
}



//边界加密不加密都可以
void RohdeCycle(int lev, amrex::Real cur_time, AmrCoreLBM& lid)
{
    amrex::Real dt = lid.Geom(lev).CellSizeArray()[0];

    if(lev == max_ref_level)
    {
        lid.ComputeParticle(lev);
    }

    lid.Boundary(lev);
    lid.Collide(lev, 0);

    if(lev < max_ref_level)
    {
        RohdeCycle(lev+1, cur_time, lid);
    }

    if(lev > coarsest_level)
    {
        lid.FillGhostLevel(lev, cur_time);
    }

    lid.CommunicateLevel(lev);
    lid.Stream(lev, 2);
    lid.SwapLevel(lev, 2);

    if(lev < max_ref_level)
    {
        lid.AverageDownGhostLevel(lev);
    } 

    if(lev == coarsest_level)
    {
        return;
    }

    cur_time += dt;

    if(lev == max_ref_level)
    {
        lid.ComputeParticle(lev);
    }
   
    lid.Boundary(lev);  
    lid.Collide(lev, 0);

    if(lev < max_ref_level)
    {
        RohdeCycle(lev+1, cur_time, lid);
    }

    lid.CommunicateLevel(lev);
    lid.Stream(lev, 2);
    lid.SwapLevel(lev, 2);

    if(lev < max_ref_level)
    {
        lid.AverageDownGhostLevel(lev);
    }        
}


//边界加密不加密都可以--Rohde循环加入非平衡态缩放
// void RohdeCycle(int lev, amrex::Real cur_time, AmrCoreLBM& lid)
// {
//     amrex::Real dt = lid.Geom(lev).CellSizeArray()[0];

//     if(lev == max_ref_level)
//     {
//         lid.ComputeParticle(lev);
//     }

//     lid.Boundary(lev);
//     lid.Collide(lev, 0);

//     if(lev < max_ref_level)
//     {
//         RohdeCycle(lev+1, cur_time, lid);
//     }

//     if(lev > coarsest_level)
//     {
//         lid.FillGhostLevel(lev, cur_time);
//         lid.InterpScale(lev, 4);
//     }

//     lid.CommunicateLevel(lev);
//     lid.Stream(lev, 2);
//     lid.SwapLevel(lev, 2);

//     if(lev < max_ref_level)
//     {
//         lid.AverageScale(lev, 4);
//         lid.AverageDownGhostLevel(lev);
//     } 

//     if(lev == coarsest_level)
//     {
//         return;
//     }

//     cur_time += dt;

//     if(lev == max_ref_level)
//     {
//         lid.ComputeParticle(lev);
//     }
   
//     lid.Boundary(lev);  
//     lid.Collide(lev, 0);

//     if(lev < max_ref_level)
//     {
//         RohdeCycle(lev+1, cur_time, lid);
//     }

//     lid.CommunicateLevel(lev);
//     lid.Stream(lev, 2);
//     lid.SwapLevel(lev, 2);

//     if(lev < max_ref_level)
//     {
//         lid.AverageScale(lev, 4);        
//         lid.AverageDownGhostLevel(lev);
//     }        
// }


//边界加密不加密都可以--Jaber循环加入非平衡态缩放
void JaberCycle(int lev, amrex::Real cur_time, AmrCoreLBM& lid)
{
    amrex::Real dt = lid.Geom(lev).CellSizeArray()[0];
  
    if(lev < max_ref_level)
    {
        lid.FillGhostLevel(lev+1, cur_time);
    }

    if(lev == max_ref_level)
    {
        lid.ComputeParticle(lev);
        lid.FillForceGhostLevel(lev, cur_time);//加一个力的填充ghost就好了
    }     

    lid.Boundary(lev);
    lid.Collide(lev, 4);
    lid.CommunicateLevel(lev);
    lid.Stream(lev, 4);
    lid.SwapLevel(lev, 4);

    if(lev < max_ref_level)
    {
        JaberCycle(lev+1, cur_time, lid);
        lid.AverageDownGhostLevel(lev);
    }

    if(lev == coarsest_level)
    {
        return;
    }

    cur_time += dt;    

    if(lev < max_ref_level)
    {        
        lid.FillGhostLevel(lev+1, cur_time);     
    }      

    if(lev == max_ref_level)
    {
        lid.ComputeParticle(lev);
        lid.FillForceGhostLevel(lev, cur_time);//加一个力的填充ghost就好了
    }

    lid.Boundary(lev);
    lid.Collide(lev, 4);
    lid.CommunicateLevel(lev);
    lid.Stream(lev, 4);
    lid.SwapLevel(lev, 4);

    if(lev < max_ref_level)
    {
        JaberCycle(lev+1, cur_time, lid);
        lid.AverageDownGhostLevel(lev);    
    }                 
    
}