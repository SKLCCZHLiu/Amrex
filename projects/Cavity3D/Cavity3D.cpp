#include <AMReX.H>
#include <AMReX_Print.H>
#include <AMReX_MultiFab.H>
#include <AMReX_MFParallelFor.H>
#include <AMReX_ParmParse.H>
#include <AMReX_PlotFileUtil.H>

#include "D3Q19.H"

int main(int argc, char* argv[])
{
    amrex::Initialize(argc,argv);

    {
        amrex::Real strt_time = ParallelDescriptor::second();  

        int stepmax = 10000;
        int plot_int = 10000;   

        int nghost = 1;


        amrex::Box     domain({AMREX_D_DECL(0, 0, 0)}, {AMREX_D_DECL(NX - 1, NY - 1, NZ - 1)});
        amrex::RealBox real_box({AMREX_D_DECL(0., 0., 0.)}, {AMREX_D_DECL(1., 1., 1.)});

        amrex::Geometry geom(domain, real_box, CoordSys::cartesian, {AMREX_D_DECL(0, 0, 0)}); 
        amrex::BoxArray ba(domain);
        amrex::DistributionMapping dm(ba);

        amrex::MultiFab fold(ba, dm, Q, nghost);
        amrex::MultiFab fnew(ba, dm, Q, nghost);
        amrex::MultiFab velocity(ba, dm, AMREX_SPACEDIM, nghost);
        amrex::MultiFab density(ba, dm, 1, nghost);

        //初始化
        for(MFIter mfi(fold, TilingIfNotGPU()); mfi.isValid(); ++mfi) 
        {
            const Box& bx = mfi.growntilebox(0);        
            const Array4<Real>& f_old = fold.array(mfi);

            amrex::ParallelFor(bx, [=]AMREX_GPU_DEVICE(int i, int j, int k)
            {
                for(int q = 0; q < Q; q++)
                {
                    f_old(i, j, k, q) = feqQian(rho0, {0.0, 0.0, 0.0}, q);
                }
            });
        }

        amrex::Real time = 0.0;

        //迭代
        for (int step = 1; step <= stepmax; ++step) 
        {
            //边界条件
            for(MFIter mfi(fold, TilingIfNotGPU()); mfi.isValid(); ++mfi)   
            {
                const Box& bx = mfi.growntilebox(0);  
                const Array4<Real>& f_new = fnew.array(mfi);
                const Array4<Real>& f_old = fold.array(mfi);

                amrex::ParallelFor(bx, [=]AMREX_GPU_DEVICE(int i, int j, int k)
                {
                    if(i == 0)
                    {
                        int x = i;
                        int y = j;
                        int z = k;

                        int x1 = x+1;
                        int y1 = y;
                        int z1 = z;

                        Real uxt1 = 0.0, uyt1 = 0.0, uzt1 = 0.0, rhot1 = 0.0, ft1 = 0.0;
                        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;

                        for(int q = 0; q < Q; q++)
                        {
                            ft1    = f_old(x1, y1, z1, q);
                            rhot1 += ft1;
                            uxt1  += ft1 * e[q][0];
                            uyt1  += ft1 * e[q][1];
                            uzt1  += ft1 * e[q][2];
                        }

                        uxt1 /= rhot1;
                        uyt1 /= rhot1;
                        uzt1 /= rhot1;

                        uxt = 0.0;
                        uyt = 0.0;
                        uzt = 0.0;

                        rhot = rhot1;

                        for(int q = 0; q < Q; q++)
                        {
                            f_new(x, y, z, q) = feqQian(rhot, {uxt, uyt, uzt}, q) + f_old(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1, uzt1}, q);
                        }                
                    }

                    if(i == NX-1)
                    {
                        int x = i;
                        int y = j;
                        int z = k;

                        int x1 = x-1;
                        int y1 = y;
                        int z1 = z;

                        Real uxt1 = 0.0, uyt1 = 0.0, uzt1 = 0.0, rhot1 = 0.0, ft1 = 0.0;
                        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;

                        for(int q = 0; q < Q; q++)
                        {
                            ft1    = f_old(x1, y1, z1, q);
                            rhot1 += ft1;
                            uxt1  += ft1 * e[q][0];
                            uyt1  += ft1 * e[q][1];
                            uzt1  += ft1 * e[q][2];
                        }

                        uxt1 /= rhot1;
                        uyt1 /= rhot1;
                        uzt1 /= rhot1;

                        uxt = 0.0;
                        uyt = 0.0;
                        uzt = 0.0;

                        rhot = rhot1;

                        for(int q = 0; q < Q; q++)
                        {
                            f_new(x, y, z, q) = feqQian(rhot, {uxt, uyt, uzt}, q) + f_old(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1, uzt1}, q);
                        }                     
                    }

                    if(j == 0)
                    {
                        int x = i;
                        int y = j;
                        int z = k;

                        int x1 = x;
                        int y1 = y+1;
                        int z1 = z;

                        Real uxt1 = 0.0, uyt1 = 0.0, uzt1 = 0.0, rhot1 = 0.0, ft1 = 0.0;
                        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;

                        for(int q = 0; q < Q; q++)
                        {
                            ft1    = f_old(x1, y1, z1, q);
                            rhot1 += ft1;
                            uxt1  += ft1 * e[q][0];
                            uyt1  += ft1 * e[q][1];
                            uzt1  += ft1 * e[q][2];
                        }

                        uxt1 /= rhot1;
                        uyt1 /= rhot1;
                        uzt1 /= rhot1;

                        uxt = 0.0;
                        uyt = 0.0;
                        uzt = 0.0;

                        rhot = rhot1;

                        for(int q = 0; q < Q; q++)
                        {
                            f_new(x, y, z, q) = feqQian(rhot, {uxt, uyt, uzt}, q) + f_old(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1, uzt1}, q);
                        }                 
                    }

                    if(j == NY-1)
                    {
                        int x = i;
                        int y = j;
                        int z = k;

                        int x1 = x;
                        int y1 = y-1;
                        int z1 = z;

                        Real uxt1 = 0.0, uyt1 = 0.0, uzt1 = 0.0, rhot1 = 0.0, ft1 = 0.0;
                        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;

                        for(int q = 0; q < Q; q++)
                        {
                            ft1    = f_old(x1, y1, z1, q);
                            rhot1 += ft1;
                            uxt1  += ft1 * e[q][0];
                            uyt1  += ft1 * e[q][1];
                            uzt1  += ft1 * e[q][2];
                        }

                        uxt1 /= rhot1;
                        uyt1 /= rhot1;
                        uzt1 /= rhot1;

                        uxt = 0.0;
                        uyt = 0.0;
                        uzt = 0.0;

                        rhot = rhot1;

                        for(int q = 0; q < Q; q++)
                        {
                            f_new(x, y, z, q) = feqQian(rhot, {uxt, uyt, uzt}, q) + f_old(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1, uzt1}, q);
                        }                  
                    }

                    if(k == 0)
                    {
                        int x = i;
                        int y = j;
                        int z = k;

                        int x1 = x;
                        int y1 = y;
                        int z1 = z+1;

                        Real uxt1 = 0.0, uyt1 = 0.0, uzt1 = 0.0, rhot1 = 0.0, ft1 = 0.0;
                        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;

                        for(int q = 0; q < Q; q++)
                        {
                            ft1    = f_old(x1, y1, z1, q);
                            rhot1 += ft1;
                            uxt1  += ft1 * e[q][0];
                            uyt1  += ft1 * e[q][1];
                            uzt1  += ft1 * e[q][2];
                        }

                        uxt1 /= rhot1;
                        uyt1 /= rhot1;
                        uzt1 /= rhot1;

                        uxt = 0.0;
                        uyt = 0.0;
                        uzt = 0.0;

                        rhot = rhot1;

                        for(int q = 0; q < Q; q++)
                        {
                            f_new(x, y, z, q) = feqQian(rhot, {uxt, uyt, uzt}, q) + f_old(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1, uzt1}, q);
                        }                  
                    }

                    if(k == NZ-1)
                    {
                        int x = i;
                        int y = j;
                        int z = k;

                        int x1 = x;
                        int y1 = y;
                        int z1 = z-1;

                        Real uxt1 = 0.0, uyt1 = 0.0, uzt1 = 0.0, rhot1 = 0.0, ft1 = 0.0;
                        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;

                        for(int q = 0; q < Q; q++)
                        {
                            ft1    = f_old(x1, y1, z1, q);
                            rhot1 += ft1;
                            uxt1  += ft1 * e[q][0];
                            uyt1  += ft1 * e[q][1];
                            uzt1  += ft1 * e[q][2];
                        }

                        uxt1 /= rhot1;
                        uyt1 /= rhot1;
                        uzt1 /= rhot1;

                        uxt = U0;
                        uyt = 0.0;
                        uzt = 0.0;

                        rhot = rhot1;

                        for(int q = 0; q < Q; q++)
                        {
                            f_new(x, y, z, q) = feqQian(rhot, {uxt, uyt, uzt}, q) + f_old(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1, uzt1}, q);
                        }                  
                    }

                });
            }     

            //碰撞
            for(MFIter mfi(fold,TilingIfNotGPU()); mfi.isValid(); ++mfi)
            {
                const Box& bx = mfi.growntilebox(0);
                const Array4<Real>& f_old = fold.array(mfi);
                const Array4<Real>& f_new = fnew.array(mfi);

                ParallelFor(bx, [=]AMREX_GPU_DEVICE(int i, int j, int k)
                {
                    Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;

                    for(int q = 0; q < Q; q++)
                    {
                        ft    = f_old(i, j, k, q);
                        rhot += ft;
                        uxt  += ft * e[q][0];
                        uyt  += ft * e[q][1];
                        uzt  += ft * e[q][2];
                    }
                    uxt /= rhot;
                    uyt /= rhot;
                    uzt /= rhot;

                    for(int q = 0; q < Q; q++)
                    {
                        f_old(i, j, k, q) = (1 - omega) * f_old(i, j, k, q) + omega * feqQian(rhot, {uxt, uyt, uzt}, q);
                    }                
                });
            }

            //通信
            fold.FillBoundary(geom.periodicity());

            //迁移
            for(MFIter mfi(fold, TilingIfNotGPU()); mfi.isValid(); ++mfi)
            {
                const auto bx = mfi.growntilebox(0);
                const Array4<Real>& f_old = fold.array(mfi);
                const Array4<Real>& f_new = fnew.array(mfi);        

                amrex::ParallelFor(bx, [=]AMREX_GPU_DEVICE(int i, int j, int k)
                {
                    if((i >= 1) && (i < NX-1) && (j >= 1) && (j < NY-1) && (k >= 1) && (k < NZ-1))
                    {
                        for(int q = 0; q < Q; q++)
                        {
                            int xm = i - e[q][0];
                            int ym = j - e[q][1];
                            int zm = k - e[q][2];

                            f_new(i, j, k, q) = f_old(xm, ym, zm, q);                   
                        }
                    }   
                });
            }         

            //交换分布函数
            std::swap(fold, fnew);
            time += dt;
            Print() << "LB step " << step << "\n";


            if(plot_int > 0 && step % plot_int == 0) 
            {
                //计算宏观量
                for (MFIter mfi(fold,TilingIfNotGPU()); mfi.isValid(); ++mfi)
                {
                    const Box& bx = mfi.growntilebox(0);        
                    Array4<Real> const& f_old = fold.array(mfi);
                    Array4<Real> const& u    = velocity.array(mfi);

                    amrex::ParallelFor(bx, [=]AMREX_GPU_DEVICE(int i, int j, int k)
                    {
                        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;

                        for(int q = 0; q < Q; q++)
                        {
                            ft    = f_old(i, j, k, q);
                            rhot += ft;
                            uxt  += ft * e[q][0];
                            uyt  += ft * e[q][1];
                            uzt  += ft * e[q][2];
                        }

                        u(i, j, k, 0)   = uxt / rhot;
                        u(i, j, k, 1)   = uyt / rhot;
                        u(i, j, k, 2)   = uzt / rhot;
                    });
                } 

                //输出
                const std::string& pltfile = amrex::Concatenate("plt",step,5);
                WriteSingleLevelPlotfile(pltfile, velocity, {"u", "v", "w"}, geom, time, step);
            }
        }

        amrex::Real stop_time = ParallelDescriptor::second() - strt_time;
        amrex::ParallelDescriptor::ReduceRealMax(stop_time);
        amrex::Print() << "Run time = " << stop_time << std::endl;
    }

    amrex::Finalize();
    return 0;
}
