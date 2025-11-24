#include "LagrangeParticleContainer.H"
#include "D3Q19.H"
#include "Kernels.H"
using namespace amrex;

void LagrangeParticleContainer::PrintParticleParm() {
    amrex::Print() << "╔══════════════════════════════════════════════════════╗" << std::endl;
    amrex::Print() << "║                      IBM PARAMETERS                  ║" << std::endl;
    amrex::Print() << "╚══════════════════════════════════════════════════════╝" << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  R      =" << std::setw(10) << std::right << R << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  rb     =" << std::setw(10) << std::right << rb << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  ds0    =" << std::setw(10) << std::right << ds0 << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  ns     =" << std::setw(10) << std::right << ns << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  nt1    =" << std::setw(10) << std::right << nt1 << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  LP_area=" << std::setw(10) << std::right << LP_area << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  LP_dr  =" << std::setw(10) << std::right << LP_dr << std::endl;

    amrex::Print() << std::setw(15) << std::left << "  rhof   =" << std::setw(10) << std::right << rhof << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  rhop   =" << std::setw(10) << std::right << rhop << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  mp     =" << std::setw(10) << std::right << Mp << std::endl;
    // amrex::Print() << std::setw(15) << std::left << "  mf     =" << std::setw(10) << std::right << Mf << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  Mp     =" << std::setw(10) << std::right << Mp_iner << std::endl;
    // amrex::Print() << std::setw(15) << std::left << "  Mf     =" << std::setw(10) << std::right << Mf_iner << std::endl;

    amrex::Print() << "╚══════════════════════════════════════════════════════╝" << std::endl;
    amrex::Print() << std::endl;
}

void LagrangeParticleContainer::InitParticle(int lev) {
    ParticleType p;
    std::array<ParticleReal, PIdx::nattribs> attribs;
    const Real* delta = Geom(0).CellSize();

    Real sita = 0.0, alpha = 0.0;
    int nt = 1;

    int num_proc = ParallelDescriptor::NProcs();

    if (ParallelDescriptor::MyProc() == ParallelDescriptor::IOProcessorNumber()) {
        for (int i = 0; i < ns; i++) {
            if (i == 0) {
                p.id() = ParticleType::NextID();
                p.cpu() = ParallelDescriptor::MyProc(); // int(p.id()%num_proc);
                p.pos(0) = (centre[0]) * delta[0];      // TODO:修改为局部坐标系失败
                p.pos(1) = (centre[1]) * delta[0];
                p.pos(2) = (centre[2]) * delta[0] + radius * dx_min;

                attribs[PIdx::xlocal] = 0.0;
                attribs[PIdx::ylocal] = 0.0;
                attribs[PIdx::zlocal] = radius * dx_min;

                attribs[PIdx::area] = LP_area * (1.0 - cos(PI / (2 * (ns - 1)))) / 2.0;
                attribs[PIdx::fx] = 0.0;
                attribs[PIdx::fy] = 0.0;
                attribs[PIdx::fz] = 0.0;
                attribs[PIdx::tx] = 0.0;
                attribs[PIdx::ty] = 0.0;
                attribs[PIdx::tz] = 0.0;

                std::pair<int, int> key{0, 0};
                auto& particle_tile = GetParticles(0)[key];

                particle_tile.push_back(p);
                particle_tile.push_back_real(attribs);
            } else if (i > 0 && i < ns - 1) {
                sita = i * PI / (ns - 1);
                nt = int(nt1 * sin(sita));

                for (int j = 0; j < nt; j++) {
                    alpha = 2 * PI / nt * (j + 1);

                    p.id() = ParticleType::NextID();
                    p.cpu() = ParallelDescriptor::MyProc(); // int(p.id()/num_proc);

                    p.pos(0) = (centre[0]) * delta[0] + radius * sin(sita) * cos(alpha) * dx_min;
                    p.pos(1) = (centre[1]) * delta[0] + radius * sin(sita) * sin(alpha) * dx_min;
                    p.pos(2) = (centre[2]) * delta[0] + radius * cos(sita) * dx_min;

                    attribs[PIdx::xlocal] = radius * sin(sita) * cos(alpha) * dx_min;
                    attribs[PIdx::ylocal] = radius * sin(sita) * sin(alpha) * dx_min;
                    attribs[PIdx::zlocal] = radius * cos(sita) * dx_min;
                    attribs[PIdx::area] = LP_area * (cos((i - 0.5) * PI / (ns - 1)) - cos((i + 0.5) * PI / (ns - 1))) / 2.0 / nt;
                    attribs[PIdx::fx] = 0.0;
                    attribs[PIdx::fy] = 0.0;
                    attribs[PIdx::fz] = 0.0;
                    attribs[PIdx::tx] = 0.0;
                    attribs[PIdx::ty] = 0.0;
                    attribs[PIdx::tz] = 0.0;

                    std::pair<int, int> key{0, 0};
                    auto& particle_tile = GetParticles(0)[key];

                    particle_tile.push_back(p);
                    particle_tile.push_back_real(attribs);
                }
            } else {
                p.id() = ParticleType::NextID();
                p.cpu() = ParallelDescriptor::MyProc(); // int(p.id()/num_proc);
                p.pos(0) = (centre[0]) * delta[0];
                p.pos(1) = (centre[1]) * delta[0];
                p.pos(2) = (centre[2]) * delta[0] - radius * dx_min;

                attribs[PIdx::xlocal] = 0.0;
                attribs[PIdx::ylocal] = 0.0;
                attribs[PIdx::zlocal] = -radius * dx_min;
                attribs[PIdx::area] = LP_area * (1.0 - cos(PI / (2 * (ns - 1)))) / 2.0;
                attribs[PIdx::fx] = 0.0;
                attribs[PIdx::fy] = 0.0;
                attribs[PIdx::fz] = 0.0;
                attribs[PIdx::tx] = 0.0;
                attribs[PIdx::ty] = 0.0;
                attribs[PIdx::tz] = 0.0;

                std::pair<int, int> key{0, 0};
                auto& particle_tile = GetParticles(0)[key];

                particle_tile.push_back(p);
                particle_tile.push_back_real(attribs);
            }
        }
    }
    Redistribute();
}

void LagrangeParticleContainer::InitChannelParticle(int lev) {
    // 在通道上下壁面均匀铺设拉格朗日点，用于 IBM 壁面处理
    // 目标间距：0.5 * dx_min（物理长度），在 x-z 平面均匀分布

    ParticleType p;
    std::array<ParticleReal, PIdx::nattribs> attribs;

    // 索引空间的起止（以单元中心为 0.5, NX-0.5 等）
    // const Real x_lo_idx = 3;
    // const Real x_hi_idx = NX - 4;
    // const Real z_lo_idx = 3;
    // const Real z_hi_idx = NZ - 4;

    const Real x_lo_idx = 0;
    const Real x_hi_idx = NX - 0.5;
    const Real z_lo_idx = 0;
    const Real z_hi_idx = NZ - 0.5;

    // 上下壁面的 j 索引位置（单元中心）
    const Real y_bot_idx = 3.5;
    const Real y_top_idx = NY - 3.5;

    // 采用索引步长（更稳妥）：索引增量 0.5，对应物理间距 0.5*dx_min
    const Real s_idx_x = 0.5;
    const Real s_idx_z = 0.5;

    // 每个点对应的“索引面积”（以格点面为单位），后续力学量会乘到物理尺度
    const Real area_per_point = s_idx_x * s_idx_z;

    // 仅在 I/O 进程上创建，再用 Redistribute 分发
    if (ParallelDescriptor::MyProc() == ParallelDescriptor::IOProcessorNumber()) {
        // 避免浮点误差导致漏掉末端
        const Real eps = 1e-12;

        for (Real xi = x_lo_idx; xi <= x_hi_idx + eps; xi += s_idx_x) {
            // 修正最后一个点，避免越界
            Real xi_clamped = amrex::min(xi, x_hi_idx);
            Real x_phys = xi_clamped * dx_min;

            for (Real zi = z_lo_idx; zi <= z_hi_idx + eps; zi += s_idx_z) {
                Real zi_clamped = amrex::min(zi, z_hi_idx);
                Real z_phys = zi_clamped * dx_min;

                // 下壁面点
                {
                    p.id() = ParticleType::NextID();
                    p.cpu() = ParallelDescriptor::MyProc();
                    p.pos(0) = x_phys;
                    p.pos(1) = y_bot_idx * dx_min;
                    p.pos(2) = z_phys;

                    // xlocal/ylocal/zlocal = 绝对位置 - centre*dx_0，使 MoveParticle 时位置保持一致
                    attribs[PIdx::xlocal] = p.pos(0) - centre[0] * dx_0;
                    attribs[PIdx::ylocal] = p.pos(1) - centre[1] * dx_0;
                    attribs[PIdx::zlocal] = p.pos(2) - centre[2] * dx_0;

                    attribs[PIdx::area] = area_per_point;
                    attribs[PIdx::fx] = 0.0;
                    attribs[PIdx::fy] = 0.0;
                    attribs[PIdx::fz] = 0.0;
                    attribs[PIdx::tx] = 0.0;
                    attribs[PIdx::ty] = 0.0;
                    attribs[PIdx::tz] = 0.0;

                    std::pair<int, int> key{0, 0};
                    auto& particle_tile = GetParticles(0)[key];
                    particle_tile.push_back(p);
                    particle_tile.push_back_real(attribs);
                }

                // 上壁面点
                {
                    p.id() = ParticleType::NextID();
                    p.cpu() = ParallelDescriptor::MyProc();
                    p.pos(0) = x_phys;
                    p.pos(1) = y_top_idx * dx_min;
                    p.pos(2) = z_phys;

                    attribs[PIdx::xlocal] = p.pos(0) - centre[0] * dx_0;
                    attribs[PIdx::ylocal] = p.pos(1) - centre[1] * dx_0;
                    attribs[PIdx::zlocal] = p.pos(2) - centre[2] * dx_0;

                    attribs[PIdx::area] = area_per_point;
                    attribs[PIdx::fx] = 0.0;
                    attribs[PIdx::fy] = 0.0;
                    attribs[PIdx::fz] = 0.0;
                    attribs[PIdx::tx] = 0.0;
                    attribs[PIdx::ty] = 0.0;
                    attribs[PIdx::tz] = 0.0;

                    std::pair<int, int> key{0, 0};
                    auto& particle_tile = GetParticles(0)[key];
                    particle_tile.push_back(p);
                    particle_tile.push_back_real(attribs);
                }
            }
        }
    }

    // 分发到对应盒与进程
    Redistribute();
}

void LagrangeParticleContainer::InitChannelParticle2(int lev) {
    // 整数索引循环 + 预分配容量（等效于 0.5*dx 的铺点密度），减少初始化阶段扩容与浮点判断开销

    ParticleType p;
    std::array<ParticleReal, PIdx::nattribs> attribs;

    // 与 InitChannelParticle 保持一致的墙面位置（索引坐标为单元中心）
    const Real y_bot_idx = 3.5;
    const Real y_top_idx = NY - 3.5;

    // 采用“半索引”步进：xi = 0, 0.5, 1.0, ..., NX-0.5，共 2*NX 个点
    // 使用整数循环避免浮点累积误差与终点判断
    const int x_count = static_cast<int>(2 * NX);
    const int z_count = static_cast<int>(2 * NZ);

    const Real half = Real(0.5);
    const Real area_per_point = half * half; // = 0.25，对应 0.5*dx 的网格面积（以索引面积计）

    // 仅在 I/O 进程生成，随后 Redistribute 分发
    if (ParallelDescriptor::MyProc() == ParallelDescriptor::IOProcessorNumber()) {
        const long total_points = 2L * static_cast<long>(x_count) * static_cast<long>(z_count); // 上下两壁

        // 预分配：通过底层 PODVector（23.09 可用）对 AoS 与 SoA 分量进行 reserve
        std::pair<int, int> key{0, 0};
        auto &particle_tile = GetParticles(0)[key];

        // AoS 预分配：tile.GetArrayOfStructs()() 返回 ParticleVector (PODVector)
        {
            auto &aos_vec = particle_tile.GetArrayOfStructs()();
            aos_vec.reserve(aos_vec.size() + static_cast<std::size_t>(total_points));
        }

        // SoA 预分配：对每个实数分量的 RealVector (PODVector) 预留容量
        {
            auto &soa = particle_tile.GetStructOfArrays();
            for (int comp = 0; comp < PIdx::nattribs; ++comp) {
                auto &rv = soa.GetRealData(comp);
                rv.reserve(rv.size() + static_cast<std::size_t>(total_points));
            }
        }

        for (int ix = 0; ix < x_count; ++ix) {
            const Real x_phys = (static_cast<Real>(ix) * half) * dx_min;
            for (int iz = 0; iz < z_count; ++iz) {
                const Real z_phys = (static_cast<Real>(iz) * half) * dx_min;

                // 下壁面点
                {
                    p.id() = ParticleType::NextID();
                    p.cpu() = ParallelDescriptor::MyProc();
                    p.pos(0) = x_phys;
                    p.pos(1) = y_bot_idx * dx_min;
                    p.pos(2) = z_phys;

                    attribs[PIdx::xlocal] = p.pos(0) - centre[0] * dx_0;
                    attribs[PIdx::ylocal] = p.pos(1) - centre[1] * dx_0;
                    attribs[PIdx::zlocal] = p.pos(2) - centre[2] * dx_0;

                    attribs[PIdx::area] = area_per_point;
                    attribs[PIdx::fx] = 0.0;
                    attribs[PIdx::fy] = 0.0;
                    attribs[PIdx::fz] = 0.0;
                    attribs[PIdx::tx] = 0.0;
                    attribs[PIdx::ty] = 0.0;
                    attribs[PIdx::tz] = 0.0;

                    particle_tile.push_back(p);
                    particle_tile.push_back_real(attribs);
                }

                // 上壁面点
                {
                    p.id() = ParticleType::NextID();
                    p.cpu() = ParallelDescriptor::MyProc();
                    p.pos(0) = x_phys;
                    p.pos(1) = y_top_idx * dx_min;
                    p.pos(2) = z_phys;

                    attribs[PIdx::xlocal] = p.pos(0) - centre[0] * dx_0;
                    attribs[PIdx::ylocal] = p.pos(1) - centre[1] * dx_0;
                    attribs[PIdx::zlocal] = p.pos(2) - centre[2] * dx_0;

                    attribs[PIdx::area] = area_per_point;
                    attribs[PIdx::fx] = 0.0;
                    attribs[PIdx::fy] = 0.0;
                    attribs[PIdx::fz] = 0.0;
                    attribs[PIdx::tx] = 0.0;
                    attribs[PIdx::ty] = 0.0;
                    attribs[PIdx::tz] = 0.0;

                    particle_tile.push_back(p);
                    particle_tile.push_back_real(attribs);
                }
            }
        }

        amrex::Print() << "InitChannelParticle2 created points: " << total_points << " (" << x_count
                       << " x " << z_count << " x 2 walls)" << std::endl;
    }

    // 分发到对应盒与进程
    Redistribute();
}

void LagrangeParticleContainer::InitParticleFromFile(int lev, const std::string object) {
    ParticleType p;
    std::array<ParticleReal, PIdx::nattribs> attribs;
    const Real* delta = Geom(0).CellSize();

    int num_proc = ParallelDescriptor::NProcs();
    int num_points;
    amrex::Real factor = 1.0;

    if (ParallelDescriptor::MyProc() == ParallelDescriptor::IOProcessorNumber()) {
        FILE* fp;

        std::string file = object + ".lpa";

        if ((fp = fopen(file.c_str(), "rb")) == NULL) {
            amrex::Print() << "error in read stl!" << std::endl;
        } else {
            printf("file is opened!\n");
            fscanf(fp, "%d", &num_points);

            for (int j = 0; j < num_points; ++j) {
                amrex::Real pos_x, pos_y, pos_z, area_tmp;
                amrex::Real dir_x, dir_y, dir_z;

                fscanf(fp, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",
                       &pos_x, &pos_y, &pos_z, &area_tmp, &dir_x, &dir_x, &dir_x);

                p.id() = ParticleType::NextID();
                p.cpu() = ParallelDescriptor::MyProc();
                p.pos(0) = (centre[0]) * delta[0] + pos_x / factor * dx_min;
                p.pos(1) = (centre[1]) * delta[0] + pos_y / factor * dx_min;
                p.pos(2) = (centre[2]) * delta[0] + pos_z / factor * dx_min;

                attribs[PIdx::xlocal] = 0.0; // 这个可以先设置成为0，目前不需要计算转矩等参数
                attribs[PIdx::ylocal] = 0.0;
                attribs[PIdx::zlocal] = 0.0;
                attribs[PIdx::area] = area_tmp / factor / factor;

                attribs[PIdx::fx] = 0.0;
                attribs[PIdx::fy] = 0.0;
                attribs[PIdx::fz] = 0.0;
                attribs[PIdx::tx] = 0.0;
                attribs[PIdx::ty] = 0.0;
                attribs[PIdx::tz] = 0.0;

                std::pair<int, int> key{0, 0};
                auto& particle_tile = GetParticles(0)[key];

                particle_tile.push_back(p);
                particle_tile.push_back_real(attribs);
            }
        }
        fclose(fp);
    }

    Redistribute();
}

void LagrangeParticleContainer::MoveParticle(int lev, amrex::Real cur_time) {
    amrex::RealVect pos_dif, pos_new;
    amrex::RealVect vel_new, angvel_new;
    amrex::RealVect G_dire{0.0, 0.0, -1.0};
    amrex::Real Fgra = (1.0 - rhof / rhop) * Mp * G;

    for (int i = 0; i < AMREX_SPACEDIM; i++) {
        // F_lub[i] = 0.0;

        vel_new[i] = (1.0 + rhof / rhop) * vel[i] - (rhof / rhop) * vel_old[i] + (F_tot[i] + Fgra * G_dire[i] + F_lub[i]) / Mp * dt_min;
        pos_new[i] = centre[i] + dt_min * 0.5 * (vel_new[i] + vel[i]) / dx_0;
        vel_old[i] = vel[i];
        vel[i] = vel_new[i];

        angvel_new[i] = (1.0 + rhof / rhop) * angvel[i] - (rhof / rhop) * angvel_old[i] + T_tot[i] * dt_min / Mp_iner;
        angvel_old[i] = angvel[i];
        angvel[i] = angvel_new[i];
    }

    // 更新拉格朗日点绝对位置
    pos_dif[0] = (pos_new[0] - centre[0]) * dx_0;
    pos_dif[1] = (pos_new[1] - centre[1]) * dx_0;
    pos_dif[2] = (pos_new[2] - centre[2]) * dx_0;

    // amrex::Print() << "dif_x = " << pos_dif[0] << ", " << "dif_y = " << pos_dif[1] << ", " << "dif_z = " << pos_dif[2] << std::endl;

    centre[0] = pos_new[0];
    centre[1] = pos_new[1];
    centre[2] = pos_new[2];

    const Real delta = Geom(lev).CellSize()[0];

    for (MyParIter pti(*this, lev); pti.isValid(); ++pti) {
        auto& particles = pti.GetArrayOfStructs();
        auto p_ptr = particles().data();
        const long n = pti.numParticles();

        auto& attribs = pti.GetAttribs();
        auto xlocal = attribs[PIdx::xlocal].data();
        auto ylocal = attribs[PIdx::ylocal].data();
        auto zlocal = attribs[PIdx::zlocal].data();

        const RealVect& centre_pos = centre;

        amrex::ParallelFor(n, [=] AMREX_GPU_DEVICE(int i) noexcept {
            p_ptr[i].pos(0) = centre_pos[0] * dx_0 + xlocal[i];
            p_ptr[i].pos(1) = centre_pos[1] * dx_0 + ylocal[i];
            p_ptr[i].pos(2) = centre_pos[2] * dx_0 + zlocal[i]; });
    }
}

amrex::RealVect LagrangeParticleContainer::ReturnCentre() {
    return centre;
}

amrex::RealVect LagrangeParticleContainer::ReturnVelocity() {
    return vel;
}

void LagrangeParticleContainer::InterpForce(int lev, amrex::MultiFab& rho_lev, amrex::MultiFab& u_lev, amrex::MultiFab& force_lev) {
    const Real delta = Geom(lev).CellSize()[0];

    for (MyParIter pti(*this, lev); pti.isValid(); ++pti) {
        auto& particles = pti.GetArrayOfStructs();
        auto p_ptr = particles().data();
        const long n = pti.numParticles();

        auto& attribs = pti.GetAttribs();
        auto fx = attribs[PIdx::fx].data();
        auto fy = attribs[PIdx::fy].data();
        auto fz = attribs[PIdx::fz].data();
        auto tx = attribs[PIdx::tx].data();
        auto ty = attribs[PIdx::ty].data();
        auto tz = attribs[PIdx::tz].data();
        auto xlocal = attribs[PIdx::xlocal].data();
        auto ylocal = attribs[PIdx::ylocal].data();
        auto zlocal = attribs[PIdx::zlocal].data();
        auto area = attribs[PIdx::area].data();

        const Array4<Real>& u = u_lev.array(pti);
        const Array4<Real>& rho = rho_lev.array(pti);
        const Array4<Real>& Ft = force_lev.array(pti);

        const RealVect& uc = vel;
        const RealVect& wc = angvel;
        const RealVect& pos = centre;

        amrex::ParallelFor(n, [=] AMREX_GPU_DEVICE(int i) noexcept { force_interp_extrap(p_ptr[i], fx[i], fy[i], fz[i], tx[i], ty[i], tz[i], xlocal[i], ylocal[i], zlocal[i], area[i], u, rho, Ft, delta, uc, wc, pos); });
    }

    // 统计转矩等颗粒受力
    using SPType = typename LagrangeParticleContainer::SuperParticleType;

    auto fx = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::fx); });

    auto fy = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::fy); });

    auto fz = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::fz); });

    auto tx = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::tx); });

    auto ty = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::ty); });
    auto tz = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::tz); });

    ParallelDescriptor::ReduceRealSum(fx);
    ParallelDescriptor::ReduceRealSum(fy);
    ParallelDescriptor::ReduceRealSum(fz);
    ParallelDescriptor::ReduceRealSum(tx);
    ParallelDescriptor::ReduceRealSum(ty);
    ParallelDescriptor::ReduceRealSum(tz);

    F_tot[0] = fx;
    F_tot[1] = fy;
    F_tot[2] = fz;
    T_tot[0] = tx;
    T_tot[1] = ty;
    T_tot[2] = tz;
    F_lub[0] = 0.0;
    F_lub[1] = 0.0;
    F_lub[2] = 0.0;
}

void LagrangeParticleContainer::InterpForceWallModel(int lev, amrex::MultiFab& rho_lev, amrex::MultiFab& u_lev, amrex::MultiFab& force_lev) {
    const Real delta = Geom(lev).CellSize()[0];

    for (MyParIter pti(*this, lev); pti.isValid(); ++pti) {
        auto& particles = pti.GetArrayOfStructs();
        auto p_ptr = particles().data();
        const long n = pti.numParticles();

        auto& attribs = pti.GetAttribs();
        auto fx = attribs[PIdx::fx].data();
        auto fy = attribs[PIdx::fy].data();
        auto fz = attribs[PIdx::fz].data();
        auto tx = attribs[PIdx::tx].data();
        auto ty = attribs[PIdx::ty].data();
        auto tz = attribs[PIdx::tz].data();
        auto xlocal = attribs[PIdx::xlocal].data();
        auto ylocal = attribs[PIdx::ylocal].data();
        auto zlocal = attribs[PIdx::zlocal].data();
        auto area = attribs[PIdx::area].data();

        const Array4<Real>& u = u_lev.array(pti);
        const Array4<Real>& rho = rho_lev.array(pti);
        const Array4<Real>& Ft = force_lev.array(pti);

        const RealVect& uc = vel;
        const RealVect& wc = angvel;
        const RealVect& pos = centre;

        amrex::ParallelFor(n, [=] AMREX_GPU_DEVICE(int i) noexcept { force_wall_model(p_ptr[i], fx[i], fy[i], fz[i], tx[i], ty[i], tz[i], xlocal[i], ylocal[i], zlocal[i], area[i],
                                                                                      u, rho, Ft, delta, uc, wc, pos); });
    }

    // 统计转矩等颗粒受力
    using SPType = typename LagrangeParticleContainer::SuperParticleType;

    auto fx = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::fx); });

    auto fy = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::fy); });

    auto fz = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::fz); });

    auto tx = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::tx); });

    auto ty = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::ty); });
    auto tz = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::tz); });

    ParallelDescriptor::ReduceRealSum(fx);
    ParallelDescriptor::ReduceRealSum(fy);
    ParallelDescriptor::ReduceRealSum(fz);
    ParallelDescriptor::ReduceRealSum(tx);
    ParallelDescriptor::ReduceRealSum(ty);
    ParallelDescriptor::ReduceRealSum(tz);

    F_tot[0] = fx;
    F_tot[1] = fy;
    F_tot[2] = fz;
    T_tot[0] = tx;
    T_tot[1] = ty;
    T_tot[2] = tz;
    F_lub[0] = 0.0;
    F_lub[1] = 0.0;
    F_lub[2] = 0.0;
}

void LagrangeParticleContainer::CollideParticle(const std::unique_ptr<LagrangeParticleContainer>& p2) {
    amrex::RealVect Fpw_lub = {0.0, 0.0, 0.0};
    amrex::RealVect Fp1_lub = {0.0, 0.0, 0.0};
    amrex::RealVect Fp2_lub = {0.0, 0.0, 0.0};

    amrex::RealVect pos_p1 = this->ReturnCentre();
    amrex::RealVect pos_p2 = p2->ReturnCentre();
    amrex::RealVect vel_p1 = this->ReturnVelocity();
    amrex::RealVect vel_p2 = p2->ReturnVelocity();

    amrex::Real lxc = pos_p1[0] - pos_p2[0]; // 位置都是lev = 0的网格下
    amrex::Real lyc = pos_p1[1] - pos_p2[1];
    amrex::Real lzc = pos_p1[2] - pos_p2[2];
    amrex::Real lc = sqrt(lxc * lxc + lyc * lyc + lzc * lzc);

    amrex::Real Ftmp = 0.0;
    amrex::Real Fgra = (1.0 - 1.0 * rhof / rhop) * Mp * G * dx_min; // 调整了大小不要忘记
    amrex::Real npp1 = (lc - R * 2.0 - safe) / safe;
    amrex::Real npp2 = (R * 2.0 - lc) / safe;

    if (lc <= (2 * R)) {
        Ftmp = std::fabs(Fgra / Epp1) * npp1 * npp1 + std::fabs(Fgra / Epp2) * npp2;
    } else if (lc <= (2 * R + safe)) {
        Ftmp = std::fabs(Fgra / Epp1) * npp1 * npp1;
    } else {
        Ftmp = 0.0;
    }

    Fp1_lub[0] += Ftmp * lxc / lc;
    Fp1_lub[1] += Ftmp * lyc / lc;
    Fp1_lub[2] += Ftmp * lzc / lc;

    Fp2_lub[0] -= Ftmp * lxc / lc;
    Fp2_lub[1] -= Ftmp * lyc / lc;
    Fp2_lub[2] -= Ftmp * lzc / lc;

    F_lub[0] = Fp1_lub[0];
    F_lub[1] = Fp1_lub[1];
    F_lub[2] = Fp1_lub[2];

    p2->SetLubVal(Fp2_lub); // 得有一个单独清零的地方
}

void LagrangeParticleContainer::CollideWall() {
    amrex::RealVect Fpw_lub = {0.0, 0.0, 0.0};

    amrex::RealVect pos_p1 = this->ReturnCentre();
    amrex::RealVect vel_p1 = this->ReturnVelocity();

    amrex::Real lxc, lyc, lzc, lc;
    amrex::Real Ftmp, npw1, npw2, xc_fict;

    amrex::Real Fgra = (1.0 - rhof / rhop) * Mp * G * dx_min;

    // 左边界
    if (pos_p1[0] <= (R + safe + 1.0)) {
        xc_fict = 1.0 - R;
        lxc = pos_p1[0] - xc_fict;
        lc = std::abs(lxc);

        npw1 = (lc - R * 2.0 - safe) / safe;
        npw2 = (R * 2.0 - lc) / safe;

        if (lc <= (2 * R)) {
            Ftmp = std::fabs(Fgra / Epw1) * npw1 * npw1 + std::fabs(Fgra / Epw2) * npw2;
        } else if (lc <= (2 * R + safe)) {
            Ftmp = std::fabs(Fgra / Epw1) * npw1 * npw1;
        } else {
            Ftmp = 0.0;
        }

        Fpw_lub[0] += Ftmp * lxc / lc;
    }
    // 右边界
    if (pos_p1[0] >= (NX - R - safe - 1.0)) {
        xc_fict = NX + R - 1.0;
        lxc = pos_p1[0] - xc_fict;
        lc = std::abs(lxc);

        npw1 = (lc - R * 2.0 - safe) / safe;
        npw2 = (R * 2.0 - lc) / safe;

        if (lc <= (2 * R)) {
            Ftmp = std::fabs(Fgra / Epw1) * npw1 * npw1 + std::fabs(Fgra / Epw2) * npw2;
        } else if (lc <= (2 * R + safe)) {
            Ftmp = std::fabs(Fgra / Epw1) * npw1 * npw1;
        } else {
            Ftmp = 0.0;
        }

        Fpw_lub[0] += Ftmp * lxc / lc;
    }

    // 后边界
    if (pos_p1[1] <= (R + safe + 1.0)) {
        xc_fict = 1.0 - R; // 这里到底是多少
        lxc = pos_p1[1] - xc_fict;
        lc = std::abs(lxc);

        npw1 = (lc - R * 2.0 - safe) / safe;
        npw2 = (R * 2.0 - lc) / safe;

        if (lc <= (2 * R)) {
            Ftmp = std::fabs(Fgra / Epw1) * npw1 * npw1 + std::fabs(Fgra / Epw2) * npw2;
        } else if (lc <= (2 * R + safe)) {
            Ftmp = std::fabs(Fgra / Epw1) * npw1 * npw1;
        } else {
            Ftmp = 0.0;
        }

        Fpw_lub[1] += Ftmp * lxc / lc;
    }

    // 前边界
    if (pos_p1[1] >= (NY - R - safe - 1.0)) {
        xc_fict = NY + R - 1.0; // 这里到底是多少
        lxc = pos_p1[1] - xc_fict;
        lc = std::abs(lxc);

        npw1 = (lc - R * 2.0 - safe) / safe;
        npw2 = (R * 2.0 - lc) / safe;

        if (lc <= (2 * R)) {
            Ftmp = std::fabs(Fgra / Epw1) * npw1 * npw1 + std::fabs(Fgra / Epw2) * npw2;
        } else if (lc <= (2 * R + safe)) {
            Ftmp = std::fabs(Fgra / Epw1) * npw1 * npw1;
        } else {
            Ftmp = 0.0;
        }

        Fpw_lub[1] += Ftmp * lxc / lc;
    }

    // 下边界
    if (pos_p1[2] <= (R + safe + 1.0)) {
        xc_fict = 1.0 - R; // 这里到底是多少
        lxc = pos_p1[2] - xc_fict;
        lc = std::abs(lxc);

        npw1 = (lc - R * 2.0 - safe) / safe;
        npw2 = (R * 2.0 - lc) / safe;

        if (lc <= (2 * R)) {
            Ftmp = std::fabs(Fgra / Epw1) * npw1 * npw1 + std::fabs(Fgra / Epw2) * npw2;
        } else if (lc <= (2 * R + safe)) {
            Ftmp = std::fabs(Fgra / Epw1) * npw1 * npw1;
        } else {
            Ftmp = 0.0;
        }

        Fpw_lub[2] += Ftmp * lxc / lc;
    }

    // 上边界
    if (pos_p1[2] >= (NZ - R - safe - 1.0)) {
        xc_fict = NZ + R - 1.0; // 这里到底是多少
        lxc = pos_p1[2] - xc_fict;
        lc = std::abs(lxc);

        npw1 = (lc - R * 2.0 - safe) / safe;
        npw2 = (R * 2.0 - lc) / safe;

        if (lc <= (2 * R)) {
            Ftmp = std::fabs(Fgra / Epw1) * npw1 * npw1 + std::fabs(Fgra / Epw2) * npw2;
        } else if (lc <= (2 * R + safe)) {
            Ftmp = std::fabs(Fgra / Epw1) * npw1 * npw1;
        } else {
            Ftmp = 0.0;
        }

        Fpw_lub[2] += Ftmp * lxc / lc;
    }

    F_lub[0] += Fpw_lub[0];
    F_lub[1] += Fpw_lub[1];
    F_lub[2] += Fpw_lub[2];
}

void LagrangeParticleContainer::SetLubVal(amrex::RealVect& force_lub) {
    F_lub[0] += force_lub[0];
    F_lub[1] += force_lub[1];
    F_lub[2] += force_lub[2];
}

void LagrangeParticleContainer::SaveVelocity(int lev, int step) {
    amrex::Real uz = vel[2] * U0;

    if (ParallelDescriptor::MyProc() == ParallelDescriptor::IOProcessorNumber()) {
        std::string filename = "data/vel_" + std::to_string(id) + ".dat";
        std::ofstream file(filename, std::ios::app);

        if (!file.is_open()) {
            std::cerr << "Cannot open the file: " << filename << std::endl;
            std::exit(1); // 错误退出
        }

        file << step << "\t" << uz << "\n";
        file.close();
    }
}

void LagrangeParticleContainer::SavePosition(int lev, int step) {
    amrex::Real pos_x = centre[0];
    amrex::Real pos_y = centre[1];
    amrex::Real pos_z = centre[2];

    if (ParallelDescriptor::MyProc() == ParallelDescriptor::IOProcessorNumber()) {
        std::string filename = "data/pos_" + std::to_string(id) + ".dat";
        std::ofstream file(filename, std::ios::app);

        if (!file.is_open()) {
            std::cerr << "Cannot open the file: " << filename << std::endl;
            std::exit(1); // 错误退出
        }

        file << step << "\t" << pos_x << "\t" << pos_y << "\t" << pos_z << "\n";
        file.close();
    }
}

void LagrangeParticleContainer::SaveFxy(int lev, int step) {
    using SPType = typename LagrangeParticleContainer::SuperParticleType;

    auto fx = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::fx); });

    auto fy = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::fy); });

    auto fz = amrex::ReduceSum(*this, [=] AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal { return p.rdata(PIdx::fz); });

    ParallelDescriptor::ReduceRealSum(fx);
    ParallelDescriptor::ReduceRealSum(fy);
    ParallelDescriptor::ReduceRealSum(fz);

    amrex::Real m = 0.5 * (rho0)*U0 * U0 * PI * D * dx_0 * D * dx_0 / 4.0;
    fx /= m;
    fy /= m;
    fz /= m;

    // if(ParallelDescriptor::MyProc() == ParallelDescriptor::IOProcessorNumber())
    // {
    //     FILE* file;

    //     if((file = fopen("CdCl.dat", "a")) == NULL)
    //     {
    //         printf("can not open the file!");
    //         exit(0);
    //     }
    //     fprintf(file, "%d\t%f\t%f\t%f\n", step, fx, fy, fz);
    //     fclose(file);
    // }

    if (ParallelDescriptor::MyProc() == ParallelDescriptor::IOProcessorNumber()) {
        std::string filename = "data/CdCl_" + std::to_string(id) + ".dat";
        std::ofstream file(filename, std::ios::app);

        if (!file.is_open()) {
            std::cerr << "Cannot open the file: " << filename << std::endl;
            std::exit(1); // 错误退出
        }

        file << step << "\t" << fx << "\t" << fy << "\t" << fz << "\n";
        file.close();
    }
}

void LagrangeParticleContainer::WriteParticle(int step) {
    std::string filename = "particle_" + std::to_string(id);
    filename += '_';
    const std::string& pltfile = amrex::Concatenate(filename, step, 5);
    WriteAsciiFile(pltfile); // 这个没啥用，只能用来调试
}
