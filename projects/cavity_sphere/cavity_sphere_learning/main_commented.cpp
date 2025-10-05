// 包含C++标准输入输出流库，用于控制台打印（例如 std::cout）
#include <iostream>

// 包含AMReX核心头文件，这是使用AMReX框架所必需的
#include <AMReX.H>
// 包含AMReX的性能分析工具，用于代码性能剖析
#include <AMReX_BLProfiler.H>
// 包含AMReX的并行描述符，用于处理MPI并行环境
#include <AMReX_ParallelDescriptor.H>

// 包含自定义的AMR-LBM核心类头文件，其中定义了AmrCoreLBM类
#include "AmrCoreLBM.H"

// 使用amrex命名空间，这样代码中就可以直接使用如 "Print"、"Real" 等AMReX定义的类型和函数，而无需写 "amrex::" 前缀
using namespace amrex;

/*
 * 函数原型声明
 * 这些函数定义了不同LBM模型和物理场景下的时间推进循环。
 * 它们采用递归方式处理多层级的自适应网格（AMR）。
 */

// Rohde模型的演化循环（适用于单个静止粒子），注释中提到它可能更适合cumulant_opt（一种优化的累积量LBM模型）
void RohdeCycle(int lev, amrex::Real cur_time, AmrCoreLBM &lid);
// Jaber模型的演化循环（适用于单个静止粒子），注释提到它更适合cumulant（标准的累积量LBM模型）
void JaberCycle(int lev, amrex::Real cur_time, AmrCoreLBM &lid);
// Rohde模型用于多颗粒自由运动场景的演化循环
void RohdeCycleMultiParticle(int lev, amrex::Real cur_time, AmrCoreLBM &lid);
// Jaber模型用于多颗粒自由运动场景的演化循环
void JaberCycleMultiParticle(int lev, amrex::Real cur_time, AmrCoreLBM &lid);

// C++程序主入口
int main(int argc, char *argv[])
{
    // 初始化AMReX环境，包括MPI和设备（如GPU）
    amrex::Initialize(argc, argv);

    // 使用大括号创建一个作用域，确保在此作用域内创建的对象（如AmrCoreLBM）在程序结束前被正确销毁
    {
        // 记录程序开始执行的墙上时间
        const Real start_time = amrex::second();

        // 声明模拟控制参数
        int max_step, regrid_int, plot_int, begin_plot;
        amrex::Real stop_time;

        // 从输入文件（通常是名为 "inputs" 的文件）中读取参数
        {
            // 创建一个ParmParse对象，用于解析输入文件
            amrex::ParmParse pp;
            // 查询（读取）全局参数
            pp.query("max_step", max_step);   // 最大时间步
            pp.query("stop_time", stop_time); // 模拟停止的物理时间
        }
        {
            // 创建另一个ParmParse对象，并指定前缀 "amr"，用于读取 "amr." 开头的参数
            amrex::ParmParse pp("amr");
            // 查询（读取）AMR相关的参数
            pp.query("regrid_int", regrid_int); // 网格重构（加密/解密）的时间步间隔
            pp.query("plot_int", plot_int);     // 输出绘图文件的时间步间隔
            pp.query("begin_plot", begin_plot); // 从哪个时间步开始输出
        }

        // 定义模拟区域的几何信息
        amrex::Geometry geom(
            // 定义第0层（最粗层）的计算域，使用整数索引表示。AMREX_D_DECL是一个宏，用于根据维度（2D/3D）生成坐标
            amrex::Box({AMREX_D_DECL(0, 0, 0)}, {AMREX_D_DECL(NX - 1, NY - 1, NZ - 1)}),
            // 定义计算域的物理尺寸（真实坐标）
            amrex::RealBox({AMREX_D_DECL(0., 0., 0.)}, {AMREX_D_DECL(nx, ny, nz)}),
            // 指定坐标系为笛卡尔坐标系
            amrex::CoordSys::cartesian,
            // 指定边界是否为周期性边界（0表示非周期性）
            {AMREX_D_DECL(0, 0, 0)});

        // 定义AMR（自适应网格细化）相关参数
        amrex::AmrInfo info{
            1,             // verbose: 输出详细信息的级别（1为开启详细输出）
            max_ref_level, // max_level: 最大细化层数
            // ref_ratio: 每一层相对于其父层的细化倍数，这里每层细化2倍
            amrex::Vector<amrex::IntVect>{(size_t)max_ref_level + 1, {AMREX_D_DECL(2, 2, 2)}},
            // blocking_factor: 每层的阻塞因子，决定网格划分的最小单元大小
            amrex::Vector<amrex::IntVect>{(size_t)max_ref_level + 1, {AMREX_D_DECL(8, 8, 8)}},
            // max_grid_size: 每层单个Box的最大尺寸
            amrex::Vector<amrex::IntVect>{(size_t)max_ref_level + 1, {AMREX_D_DECL(128, 128, 128)}}};

        // 初始化当前物理时间
        amrex::Real cur_time = 0.0;

        // 创建核心的AMR-LBM模拟对象
        AmrCoreLBM lid(geom, info);
        // 初始化网格结构
        lid.InitMesh(cur_time);
        // 打印网格信息
        lid.PrintMeshInfo();
        // 打印LBM模型参数
        lid.PrintLbmParm();
        // 初始化粒子信息
        lid.InitParticle(max_ref_level);
        // 初始化用于计算压力的点
        lid.InitCpPoint(max_ref_level);
        // 打印粒子参数
        lid.PrintParticleParm();

        // 主时间循环
        for (int step = 1; step <= max_step && cur_time < stop_time; step++)
        {
            // 打印当前步数信息
            amrex::Print() << "STEP " << step << "starts ..." << std::endl;

            // 这部分被注释掉了，是用于动态自适应网格的功能
            // if(step >= 0 && step % regrid_int == 0)
            // {
            //     lid.AverageDownValid(); // 将有效区域从细网格平均到粗网格
            //     lid.FindCentre();       // 寻找加密中心
            //     lid.RefineMesh(cur_time); // 根据标记的单元进行网格加密/解密
            //     lid.RedistributeParticle(); // 在新的网格结构上重新分布粒子
            // }

            /*---------------用于计算静止圆球绕流----------------------------------*/
            // RohdeCycle(0, cur_time, lid); // 如果使用Rohde模型，则取消此行注释
            JaberCycle(0, cur_time, lid);       // 调用Jaber模型的演化循环，从第0层开始
            lid.ReduceFxy(max_ref_level, step); // 计算并累加作用在粒子上的力
                                                /*--------------------------------------------------------------------*/

            /*---------------用于计算多颗粒自由运动----------------------------------*/
            // RohdeCycleMultiParticle(0, cur_time, lid); // 如果模拟多颗粒，则使用此函数
            // JaberCycleMultiParticle(0, cur_time, lid);

            // lid.SaveParticlePosition(0, step); // 保存粒子位置
            // lid.SaveParticleVelocity(0, step); // 保存粒子速度
            // lid.SaveParticleDistance(0, step); // 保存粒子间距
            /*--------------------------------------------------------------------*/

            // 更新物理时间
            cur_time += dt_0;

            // 这部分被注释掉了，用于在特定时间步范围内计算压力系数
            // if(step >= 98000 && step <= 100000 && step % 100 == 0)
            // {
            //     lid.ComputeCp(max_ref_level, step);
            // }

            // 判断是否到达输出时间步
            if (step >= begin_plot && step % plot_int == 0)
            {
                lid.PrintMeshInfo();                   // 打印网格信息
                lid.ComputeMacro();                    // 计算宏观物理量（如速度、密度）
                lid.ComputeVorticity(cur_time);        // 计算涡量
                lid.WriteVelocityFile(step, cur_time); // 将速度场写入文件
                // 下面是被注释掉的其他可能的输出选项
                // lid.WriteDensityFile(step, cur_time);
                // lid.ComputeCp(max_ref_level, step);
                // lid.WriteMultiParticleFile(step, cur_time);
                // lid.WriteVelocityFile(step, cur_time, max_ref_level);
                // lid.WriteParticleFile(step, cur_time);
                // lid.WriteVorticityFile(step, cur_time);
                // lid.WriteVelocityFileWithParticle(step, cur_time);
            }
        }

        // 计算总运行时间
        amrex::Real end_total = amrex::second() - start_time;
        if (lid.Verbose())
        {
            // 在所有进程中找到最大的运行时间，并由IO主进程打印
            ParallelDescriptor::ReduceRealMax(end_total, ParallelDescriptor::IOProcessorNumber());
            amrex::Print() << "Total Time : " << end_total << std::endl;
        }
    }

    // 结束AMReX环境，释放资源
    amrex::Finalize();

    return 0;
}

/*---------------用于计算静止圆球绕流的Rohde模型演化循环----------------------------------*/
void RohdeCycle(int lev, amrex::Real cur_time, AmrCoreLBM &lid)
{
    // 获取当前层级的格子大小，也即时间步长
    amrex::Real dt = lid.Geom(lev).CellSizeArray()[0];

    // 如果当前是最细层，则进行粒子相关的计算（如流固耦合力）
    if (lev == max_ref_level)
    {
        lid.ComputeParticle(lev);
    }

    // 应用物理边界条件
    lid.Boundary(lev);
    // 执行LBM的碰撞步
    lid.Collide(lev, 0);

    // 如果当前层不是最细层，则递归调用下一层（更细的层）的演化循环
    if (lev < max_ref_level)
    {
        RohdeCycle(lev + 1, cur_time, lid);
    }

    // 如果当前层不是最粗层，则从其父层（更粗的层）填充Ghost单元的数据（Coarse-Fine边界处理）
    if (lev > coarsest_level)
    {
        lid.FillGhostLevel(lev, cur_time, 0); // 从更粗的层获取数据，填充到本层
    }

    // 在当前层级的所有处理器之间通信，交换边界数据
    lid.CommunicateLevel(lev);
    // 执行LBM的迁移步
    lid.Stream(lev, 2);
    // 交换新旧分布函数，为下一次迭代做准备
    lid.SwapLevel(lev, 2);

    // 如果当前层不是最细层，则将子层（更细的层）的数据平均到当前层的Ghost单元（Fine-Coarse边界处理）
    if (lev < max_ref_level)
    {
        lid.AverageDownGhostLevel(lev, 0); // 将更细层(lev+1)的数据平均，更新本层(lev)
    }

    // 如果是第0层（最粗层），则一个完整的双步递归循环结束
    if (lev == coarsest_level)
    {
        return;
    }

    // 更新时间（这是Rohde模型的第二步）
    cur_time += dt;

    // 重复上述过程，完成一个完整的LBM时间步
    if (lev == max_ref_level)
    {
        lid.ComputeParticle(lev);
    }

    lid.Boundary(lev);
    lid.Collide(lev, 0);

    if (lev < max_ref_level)
    {
        RohdeCycle(lev + 1, cur_time, lid);
    }

    lid.CommunicateLevel(lev);
    lid.Stream(lev, 2);
    lid.SwapLevel(lev, 2);

    if (lev < max_ref_level)
    {
        lid.AverageDownGhostLevel(lev, 0);
    }
}

/*---------------用于计算静止圆球绕流的Jaber模型演化循环----------------------------------*/
// Jaber模型的结构与Rohde类似，但它在递归和数据交换的顺序上有所不同，
// 并且它在碰撞和迁移步骤中处理数据的方式可能也不同（由Collide和Stream函数内部实现决定）。
void JaberCycle(int lev, amrex::Real cur_time, AmrCoreLBM &lid)
{
    amrex::Real dt = lid.Geom(lev).CellSizeArray()[0];

    // Jaber模型先处理Coarse-Fine边界
    if (lev < max_ref_level)
    {
        lid.FillGhostLevel(lev + 1, cur_time, 1);
    }

    // 在最细层处理粒子和力
    if (lev == max_ref_level)
    {
        lid.ComputeParticle(lev);
        lid.FillForceGhostLevel(lev, cur_time); // 填充力的Ghost单元
    }

    // 碰撞、通信、迁移
    lid.Boundary(lev);
    lid.Collide(lev, 4);
    lid.CommunicateLevel(lev);
    lid.Stream(lev, 4);
    lid.SwapLevel(lev, 4);

    // 递归到下一层，然后处理Fine-Coarse边界
    if (lev < max_ref_level)
    {
        JaberCycle(lev + 1, cur_time, lid);
        lid.AverageDownGhostLevel(lev, 1);
    }

    if (lev == coarsest_level)
    {
        return;
    }

    // 更新时间，开始第二步
    cur_time += dt;

    // 重复相同的流程
    if (lev < max_ref_level)
    {
        lid.FillGhostLevel(lev + 1, cur_time, 1);
    }

    if (lev == max_ref_level)
    {
        lid.ComputeParticle(lev);
        lid.FillForceGhostLevel(lev, cur_time);
    }

    lid.Boundary(lev);
    lid.Collide(lev, 4);
    lid.CommunicateLevel(lev);
    lid.Stream(lev, 4);
    lid.SwapLevel(lev, 4);

    if (lev < max_ref_level)
    {
        // 注意这里是再次递归调用，这似乎与RohdeCycle不同，可能是一个错误或者特定的算法设计
        // 正常的实现应该是只在Fine-Coarse边界做数据平均
        JaberCycle(lev + 1, cur_time, lid);
        lid.AverageDownGhostLevel(lev, 1);
    }
}

/*---------------用于计算多颗粒自由运动的Rohde模型演化循环----------------------------------*/
// 这个函数在单粒子版本的基础上，增加了处理多颗粒运动的逻辑
void RohdeCycleMultiParticle(int lev, amrex::Real cur_time, AmrCoreLBM &lid)
{
    amrex::Real dt = lid.Geom(lev).CellSizeArray()[0];

    if (lev == max_ref_level)
    {
        lid.ComputeParticle(lev);            // 计算流固耦合力
        lid.LubForceParticle(lev, cur_time); // 计算润滑力
        lid.MoveParticle(lev, cur_time);     // 根据合力更新粒子位置和速度
    }

    // 后续流程与单粒子版本类似
    lid.Boundary(lev);
    lid.Collide(lev, 0);

    if (lev < max_ref_level)
    {
        RohdeCycleMultiParticle(lev + 1, cur_time, lid);
    }

    if (lev > coarsest_level)
    {
        lid.FillGhostLevel(lev, cur_time, 0);
    }

    lid.CommunicateLevel(lev);
    lid.Stream(lev, 2);
    lid.SwapLevel(lev, 2);

    if (lev < max_ref_level)
    {
        lid.AverageDownGhostLevel(lev, 0);
    }

    if (lev == coarsest_level)
    {
        return;
    }

    cur_time += dt;

    if (lev == max_ref_level)
    {
        lid.ComputeParticle(lev);
        lid.LubForceParticle(lev, cur_time);
        lid.MoveParticle(lev, cur_time);
    }

    lid.Boundary(lev);
    lid.Collide(lev, 0);

    if (lev < max_ref_level)
    {
        RohdeCycleMultiParticle(lev + 1, cur_time, lid);
    }

    lid.CommunicateLevel(lev);
    lid.Stream(lev, 2);
    lid.SwapLevel(lev, 2);

    if (lev < max_ref_level)
    {
        lid.AverageDownGhostLevel(lev, 0);
    }
}

/*---------------用于计算多颗粒自由运动的Jaber模型演化循环----------------------------------*/
// Jaber模型的多颗粒版本
void JaberCycleMultiParticle(int lev, amrex::Real cur_time, AmrCoreLBM &lid)
{
    amrex::Real dt = lid.Geom(lev).CellSizeArray()[0];

    if (lev < max_ref_level)
    {
        lid.FillGhostLevel(lev + 1, cur_time, 1);
    }

    if (lev == max_ref_level)
    {
        lid.ComputeParticle(lev);
        lid.FillForceGhostLevel(lev, cur_time);
        lid.LubForceParticle(lev, cur_time);
        lid.MoveParticle(lev, cur_time);
    }

    lid.Boundary(lev);
    lid.Collide(lev, 4);
    lid.CommunicateLevel(lev);
    lid.Stream(lev, 4);
    lid.SwapLevel(lev, 4);

    if (lev < max_ref_level)
    {
        JaberCycleMultiParticle(lev + 1, cur_time, lid);
        lid.AverageDownGhostLevel(lev, 1);
    }

    if (lev == coarsest_level)
    {
        return;
    }

    cur_time += dt;

    if (lev < max_ref_level)
    {
        lid.FillGhostLevel(lev + 1, cur_time, 1);
    }

    if (lev == max_ref_level)
    {
        lid.ComputeParticle(lev);
        lid.FillForceGhostLevel(lev, cur_time);
        lid.LubForceParticle(lev, cur_time);
        lid.MoveParticle(lev, cur_time);
    }

    lid.Boundary(lev);
    lid.Collide(lev, 4);
    lid.CommunicateLevel(lev);
    lid.Stream(lev, 4);
    lid.SwapLevel(lev, 4);

    if (lev < max_ref_level)
    {
        JaberCycleMultiParticle(lev + 1, cur_time, lid);
        lid.AverageDownGhostLevel(lev, 1);
    }
}
