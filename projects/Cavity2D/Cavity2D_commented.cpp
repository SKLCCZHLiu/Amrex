#include <AMReX.H>
#include <AMReX_Print.H>
#include <AMReX_MultiFab.H>
#include <AMReX_MFParallelFor.H>
#include <AMReX_ParmParse.H>
#include <AMReX_PlotFileUtil.H>

#include "D2Q9.H"

// AMReX (Adaptive Mesh Refinement EXperimental) 是一个用于在块状结构化自适应网格上
// 构建大规模并行应用程序的软件框架。这个例子展示了如何使用AMReX来实现一个
// 二维格子玻尔兹曼方法（LBM）来模拟顶盖驱动方腔流问题。

int main(int argc, char* argv[])
{
    // 使用amrex::Initialize来初始化AMReX环境和MPI。
    // 这是任何AMReX程序的入口点。
    amrex::Initialize(argc,argv);

    // 使用一个花括号{}创建一个作用域。当程序离开这个作用域时，
    // 在其中创建的所有AMReX相关对象（如MultiFab）的析构函数将被自动调用。
    // 这是一种良好的资源管理实践。
    {
        // --- 1. 定义模拟参数 ---

        // 使用amrex::ParallelDescriptor::second()获取当前时间，用于计算总运行时间。
        // 这是进行性能分析的常用方法。
        amrex::Real strt_time = amrex::ParallelDescriptor::second();

        // 定义模拟的最大时间步数。
        int stepmax = 10000;
        // 定义输出绘图文件的频率。每10000步输出一次。
        int plot_int = 10000;   

        // 定义"ghost cells"（晕圈或影子单元）的层数。
        // Ghost cells是围绕每个网格块的额外数据层，用于存储相邻网格块的数据副本，
        // 以便在并行计算中高效地处理跨边界的数据依赖。这是并行计算的核心概念。
        int nghost = 1;

        // --- 2. 设置计算区域的几何信息 ---

        // amrex::Box: 定义一个整数索引空间中的矩形区域。
        // 这里定义了整个计算域的索引范围，从(0,0,0)到(NX-1, NY-1, NZ-1)。
        // AMREX_D_DECL是一个宏，会根据编译时指定的维度（2D或3D）自动展开。
        amrex::Box domain({AMREX_D_DECL(0, 0, 0)}, {AMREX_D_DECL(NX - 1, NY - 1, NZ - 1)});
        
        // amrex::RealBox: 定义物理空间中的一个矩形区域。
        // 这里将计算域的物理尺寸定义为从(0,0,0)到(1,1,1)的单位立方体。
        amrex::RealBox real_box({AMREX_D_DECL(0., 0., 0.)}, {AMREX_D_DECL(1., 1., 1.)});

        // amrex::Geometry: 定义一个几何对象，它包含了索引空间、物理空间、坐标系
        // 以及周期性边界的所有信息。这是连接模拟网格和物理问题的桥梁。
        // 最后一个参数 `{AMREX_D_DECL(0, 0, 0)}` 表示所有方向都是非周期的。
        amrex::Geometry geom(domain, real_box, amrex::CoordSys::cartesian, {AMREX_D_DECL(0, 0, 0)}); 
        
        // amrex::BoxArray: 定义了整个计算域如何被分解成一系列不相交的Box。
        // 在并行计算中，这些Box可以被分配给不同的处理器。这里初始时只有一个Box。
        amrex::BoxArray ba(domain);
        
        // amrex::DistributionMapping: 定义了BoxArray中的每个Box如何映射到MPI进程。
        // 它负责数据的并行分布和负载均衡。
        amrex::DistributionMapping dm(ba);

        // --- 3. 创建数据容器 (MultiFab) ---

        // amrex::MultiFab: 是AMReX的核心数据容器，它是一个分布式的、包含多个
        // FArrayBox（FAB）的集合，每个FAB对应BoxArray中的一个Box。
        // 参数:
        //   ba: BoxArray，定义了网格结构。
        //   dm: DistributionMapping，定义了并行分布。
        //   Q:  每个网格点的数据分量数 (对于D2Q9模型，Q=9)。
        //   nghost: ghost cells的层数。

        // fold: 存储当前时间步的分布函数 f_old。
        amrex::MultiFab fold(ba, dm, Q, nghost);
        // fnew: 存储下一个时间步的分布函数 f_new。
        amrex::MultiFab fnew(ba, dm, Q, nghost);
        // velocity: 存储宏观速度场 (2个分量: u_x, u_y)。
        amrex::MultiFab velocity(ba, dm, AMREX_SPACEDIM, nghost);
        // density: 存储宏观密度场 (1个分量)。
        amrex::MultiFab density(ba, dm, 1, nghost);

        // --- 4. 初始化流场 ---
        // 目标：将所有格点的分布函数初始化为对应于静止流体（速度为0）的平衡态分布。

        // MFIter (MultiFab Iterator): 用于遍历一个MultiFab中的所有网格块(Box)。
        // 这是在AMReX中进行并行计算的标准循环方式。
        // TilingIfNotGPU(): 一个优化选项，如果使用GPU，AMReX会选择合适的tile大小。
        for(amrex::MFIter mfi(fold, amrex::TilingIfNotGPU()); mfi.isValid(); ++mfi) 
        {
            // 获取当前迭代器mfi指向的网格块的索引范围。
            const amrex::Box& bx = mfi.growntilebox(0);        
            
            // 获取指向当前网格块数据的、可直接索引的视图(Array4)。
            // 这是一个轻量级的代理对象，让我们能像操作普通多维数组一样操作MultiFab的数据。
            // 使用引用(&)可以避免不必要的数据拷贝，提高效率。
            const amrex::Array4<amrex::Real>& f_old = fold.array(mfi);

            // amrex::ParallelFor: 一个并行的for循环，可以在CPU线程或GPU上执行。
            // 它会遍历bx范围内的所有格点(i,j,k)。
            // [=]AMREX_GPU_DEVICE(...) 是一个lambda表达式，定义了在每个格点上执行的操作。
            amrex::ParallelFor(bx, [=]AMREX_GPU_DEVICE(int i, int j, int k)
            {
                // 遍历9个离散速度方向。
                for(int q = 0; q < Q; q++)
                {
                    // 调用feqQian函数计算平衡分布函数，并赋值给f_old。
                    // 初始密度为rho0，初始速度为(0.0, 0.0)。
                    f_old(i, j, k, q) = feqQian(rho0, {0.0, 0.0}, q);
                }
            });
        }

        // 初始化模拟时间。
        amrex::Real time = 0.0;

        // --- 5. 时间推进主循环 ---
        for (int step = 1; step <= stepmax; ++step) 
        {
            // --- 5.1 边界条件处理 ---
            // 在LBM中，边界条件通常在碰撞和迁移步骤之间或之前处理。
            // 这里的实现方式是在每个时间步开始时，根据邻近格点的信息来设定边界格点的分布函数。
            // 这种方法被称为非平衡态外推格式，用于处理速度和压力边界。
            for(amrex::MFIter mfi(fold, amrex::TilingIfNotGPU()); mfi.isValid(); ++mfi)   
            {
                const amrex::Box& bx = mfi.growntilebox(0);  
                const amrex::Array4<amrex::Real>& f_new = fnew.array(mfi);
                const amrex::Array4<amrex::Real>& f_old = fold.array(mfi);

                amrex::ParallelFor(bx, [=]AMREX_GPU_DEVICE(int i, int j, int k)
                {
                    // 左边界 (i=0)，设置为固壁（速度为0）。
                    if(i == 0)
                    {
                        // ... (此处省略具体实现细节) ...
                        // 核心思想: 使用内部第一层(i=1)的流场信息，外推出边界(i=0)的分布函数，
                        // 同时强制边界速度为(0,0)。
                        int x = i; int y = j; int z = k;
                        int x1 = x+1; int y1 = y; int z1 = z;
                        amrex::Real uxt1 = 0.0, uyt1 = 0.0, rhot1 = 0.0;
                        for(int q = 0; q < Q; q++) {
                            amrex::Real ft1 = f_old(x1, y1, z1, q);
                            rhot1 += ft1; uxt1 += ft1 * e[q][0]; uyt1 += ft1 * e[q][1];
                        }
                        uxt1 /= rhot1; uyt1 /= rhot1;
                        amrex::Real uxt = 0.0, uyt = 0.0, rhot = rhot1;
                        for(int q = 0; q < Q; q++) {
                            f_new(x, y, z, q) = feqQian(rhot, {uxt, uyt}, q) + f_old(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1}, q);
                        }                
                    }

                    // 右边界 (i=NX-1)，设置为固壁（速度为0）。
                    if(i == NX-1)
                    {
                        // ... (实现同上，但使用内部层i=NX-2的信息) ...
                        int x = i; int y = j; int z = k;
                        int x1 = x-1; int y1 = y; int z1 = z;
                        amrex::Real uxt1 = 0.0, uyt1 = 0.0, rhot1 = 0.0;
                        for(int q = 0; q < Q; q++) {
                            amrex::Real ft1 = f_old(x1, y1, z1, q);
                            rhot1 += ft1; uxt1 += ft1 * e[q][0]; uyt1 += ft1 * e[q][1];
                        }
                        uxt1 /= rhot1; uyt1 /= rhot1;
                        amrex::Real uxt = 0.0, uyt = 0.0, rhot = rhot1;
                        for(int q = 0; q < Q; q++) {
                            f_new(x, y, z, q) = feqQian(rhot, {uxt, uyt}, q) + f_old(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1}, q);
                        }                
                    }

                    // 下边界 (j=0)，设置为固壁（速度为0）。
                    if(j == 0)
                    {
                        // ... (实现同上，但使用内部层j=1的信息) ...
                        int x = i; int y = j; int z = k;
                        int x1 = x; int y1 = y+1; int z1 = z;
                        amrex::Real uxt1 = 0.0, uyt1 = 0.0, rhot1 = 0.0;
                        for(int q = 0; q < Q; q++) {
                            amrex::Real ft1 = f_old(x1, y1, z1, q);
                            rhot1 += ft1; uxt1 += ft1 * e[q][0]; uyt1 += ft1 * e[q][1];
                        }
                        uxt1 /= rhot1; uyt1 /= rhot1;
                        amrex::Real uxt = 0.0, uyt = 0.0, rhot = rhot1;
                        for(int q = 0; q < Q; q++) {
                            f_new(x, y, z, q) = feqQian(rhot, {uxt, uyt}, q) + f_old(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1}, q);
                        }                
                    }

                    // 上边界 (j=NY-1)，顶盖驱动边界，设置为水平速度U0。
                    if(j == NY-1)
                    {
                        // ... (实现同上，但强制边界速度为(U0, 0)) ...
                        int x = i; int y = j; int z = k;
                        int x1 = x; int y1 = y-1; int z1 = z;
                        amrex::Real uxt1 = 0.0, uyt1 = 0.0, rhot1 = 0.0;
                        for(int q = 0; q < Q; q++) {
                            amrex::Real ft1 = f_old(x1, y1, z1, q);
                            rhot1 += ft1; uxt1 += ft1 * e[q][0]; uyt1 += ft1 * e[q][1];
                        }
                        uxt1 /= rhot1; uyt1 /= rhot1;
                        amrex::Real uxt = U0, uyt = 0.0, rhot = rhot1;
                        for(int q = 0; q < Q; q++) {
                            f_new(x, y, z, q) = feqQian(rhot, {uxt, uyt}, q) + f_old(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1}, q);
                        }                
                    }
                });
            }     

            // --- 5.2 碰撞步骤 (Collision) ---
            // 在每个格点上，根据当前分布函数计算宏观量（密度、速度），
            // 然后将分布函数向平衡态松弛。
            for(amrex::MFIter mfi(fold,amrex::TilingIfNotGPU()); mfi.isValid(); ++mfi)
            {
                const amrex::Box& bx = mfi.growntilebox(0);
                const amrex::Array4<amrex::Real>& f_old = fold.array(mfi);
                const amrex::Array4<amrex::Real>& f_new = fnew.array(mfi);

                amrex::ParallelFor(bx, [=]AMREX_GPU_DEVICE(int i, int j, int k)
                {
                    // a. 计算宏观密度和速度
                    amrex::Real uxt = 0.0, uyt = 0.0, rhot = 0.0;
                    for(int q = 0; q < Q; q++)
                    {
                        amrex::Real ft = f_old(i, j, k, q);
                        rhot += ft;
                        uxt  += ft * e[q][0];
                        uyt  += ft * e[q][1];
                    }
                    uxt /= rhot;
                    uyt /= rhot;

                    // b. BGK碰撞模型: f_new = f_old + omega * (f_eq - f_old)
                    for(int q = 0; q < Q; q++)
                    {
                        f_old(i, j, k, q) = (1 - omega) * f_old(i, j, k, q) + omega * feqQian(rhot, {uxt, uyt}, q);
                    }                
                });
            }

            // --- 5.3 通信 (Communication) ---
            // 这是并行计算的关键步骤！
            // 调用FillBoundary()会触发MPI通信，用相邻网格块的数据填充当前块的ghost cells。
            // 这样，在下一步的迁移操作中，每个进程就能在本地访问到所有需要的邻居数据，
            // 而无需在计算循环中进行等待和通信。
            fold.FillBoundary(geom.periodicity());

            // --- 5.4 迁移步骤 (Streaming) ---
            // 模拟流体粒子沿着离散速度方向移动到相邻格点。
            // f_new(i,j) = f_old(i-cx, j-cy)
            for(amrex::MFIter mfi(fold, amrex::TilingIfNotGPU()); mfi.isValid(); ++mfi)
            {
                const auto bx = mfi.growntilebox(0);
                const amrex::Array4<amrex::Real>& f_old = fold.array(mfi);
                const amrex::Array4<amrex::Real>& f_new = fnew.array(mfi);        

                amrex::ParallelFor(bx, [=]AMREX_GPU_DEVICE(int i, int j, int k)
                {
                    // 只在内部流体域进行迁移
                    if((i >= 1) && (i < NX-1) && (j >= 1) && (j < NY-1))
                    {
                        for(int q = 0; q < Q; q++)
                        {
                            // 计算上游邻居格点的坐标
                            int xm = i - e[q][0];
                            int ym = j - e[q][1];
                            int zm = 0;

                            // 从上游邻居格点(xm, ym)获取分布函数，并更新到当前格点(i,j)
                            // 注意：如果(xm,ym)位于ghost cell区域，由于上一步FillBoundary的存在，
                            // f_old(xm,ym,zm,q)依然可以被正确访问。
                            f_new(i, j, k, q) = f_old(xm, ym, zm, q);                   
                        }
                    }   
                });
            }         

            // --- 5.5 交换新旧分布函数 ---
            // 使用std::swap高效地交换fold和fnew的指针，避免了大规模数据拷贝。
            // 这样，上一步计算出的fnew就变成了下一步的fold。
            std::swap(fold, fnew);
            
            // 更新模拟时间并打印当前步数。
            time += dt;
            amrex::Print() << "LB step " << step << "\n";

            // --- 5.6 数据输出 ---
            // 如果达到指定的输出间隔，则计算宏观量并写入绘图文件。
            if(plot_int > 0 && step % plot_int == 0) 
            {
                // a. 计算宏观速度场，并存入velocity这个MultiFab中。
                for (amrex::MFIter mfi(fold,amrex::TilingIfNotGPU()); mfi.isValid(); ++mfi)
                {
                    const amrex::Box& bx = mfi.growntilebox(0);        
                    amrex::Array4<amrex::Real> const& f_old = fold.array(mfi);
                    amrex::Array4<amrex::Real> const& u    = velocity.array(mfi);

                    amrex::ParallelFor(bx, [=]AMREX_GPU_DEVICE(int i, int j, int k)
                    {
                        amrex::Real uxt = 0.0, uyt = 0.0, rhot = 0.0;
                        for(int q = 0; q < Q; q++) {
                            amrex::Real ft = f_old(i, j, k, q);
                            rhot += ft; uxt += ft * e[q][0]; uyt += ft * e[q][1];
                        }
                        u(i, j, k, 0) = uxt / rhot;
                        u(i, j, k, 1) = uyt / rhot;
                    });
                } 

                // b. 将速度场数据写入Plotfile。
                // Plotfile是AMReX的标准输出格式，可以被VisIt, ParaView等可视化软件读取。
                const std::string& pltfile = amrex::Concatenate("plt",step,5);
                amrex::WriteSingleLevelPlotfile(pltfile, velocity, {"u", "v"}, geom, time, step);
            }
        }

        // --- 6. 计算并打印总运行时间 ---
        amrex::Real stop_time = amrex::ParallelDescriptor::second() - strt_time;
        // 在所有进程中找到最长的运行时间，以确保所有工作都已完成。
        amrex::ParallelDescriptor::ReduceRealMax(stop_time);
        amrex::Print() << "Run time = " << stop_time << std::endl;
    }

    // 清理AMReX和MPI环境。
    amrex::Finalize();
    return 0;
}
