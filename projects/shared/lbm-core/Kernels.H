#ifndef KERNELS_H_
#define KERNELS_H_

#include <AMReX_Box.H>
#include <AMReX_FArrayBox.H>
#include <AMReX_Geometry.H>
#include <AMReX_PhysBCFunct.H>

#include "D3Q19.H"
#define KERNEL_ABS std::abs

using namespace amrex;

static constexpr Real c1o2 = 1.0 / 2.0;
static constexpr Real c3o2 = 3.0 / 2.0;
static constexpr Real c1o3 = 1.0 / 3.0;
static constexpr Real c2o3 = 2.0 / 3.0;
static constexpr Real c1o4 = 1.0 / 4.0;
static constexpr Real c3o4 = 3.0 / 4.0;
static constexpr Real c1o6 = 1.0 / 6.0;
static constexpr Real c1o7 = 1.0 / 7.0;
static constexpr Real c1o8 = 1.0 / 8.0;
static constexpr Real c1o9 = 1.0 / 9.0;
static constexpr Real c2o9 = 2.0 / 9.0;
static constexpr Real c4o9 = 4.0 / 9.0;
static constexpr Real c4o10 = 4.0 / 10.0;
static constexpr Real c1o10 = 1.0 / 10.0;
static constexpr Real c1o12 = 1.0 / 12.0;
static constexpr Real c1o16 = 1.0 / 16.0;
static constexpr Real c3o16 = 3.0 / 16.0;
static constexpr Real c9o16 = 9.0 / 16.0;
static constexpr Real c1o18 = 1.0 / 18.0;
static constexpr Real c1o20 = 1.0 / 20.0;
static constexpr Real c19o20 = 19.0 / 20.0;
static constexpr Real c21o20 = 21.0 / 20.0;
static constexpr Real c1o24 = 1.0 / 24.0;
static constexpr Real c1o27 = 1.0 / 27.0;
static constexpr Real c3o32 = 3.0 / 32.0;
static constexpr Real c4o32 = 4.0 / 32.0;
static constexpr Real c1o36 = 1.0 / 36.0;
static constexpr Real c1o48 = 1.0 / 48.0;
static constexpr Real c1o64 = 1.0 / 64.0;
static constexpr Real c3o64 = 3.0 / 64.0;
static constexpr Real c9o64 = 9.0 / 64.0;
static constexpr Real c27o64 = 27.0 / 64.0;
static constexpr Real c1o66 = 1.0 / 66.0;
static constexpr Real c1o72 = 1.0 / 72.0;
static constexpr Real c1o264 = 1.0 / 264.0;
static constexpr Real c8o27 = 8.0 / 27.0;
static constexpr Real c2o27 = 2.0 / 27.0;
static constexpr Real c1o54 = 1.0 / 54.0;
static constexpr Real c1o100 = 1.0 / 100.0;
static constexpr Real c99o100 = 99.0 / 100.0;
static constexpr Real c1o126 = 1.0 / 126.0;
static constexpr Real c1o216 = 1.0 / 216.0;
static constexpr Real c5o4 = 5.0 / 4.0;
static constexpr Real c4o3 = 4.0 / 3.0;
static constexpr Real c9o4 = 9.0 / 4.0;
static constexpr Real c5o2 = 5.0 / 2.0;
static constexpr Real c9o2 = 9.0 / 2.0;

static constexpr Real c0o1 = 0.;
static constexpr Real c1o1 = 1.;
static constexpr Real c2o1 = 2.;
static constexpr Real c3o1 = 3.;
static constexpr Real c4o1 = 4.;
static constexpr Real c5o1 = 5.;
static constexpr Real c6o1 = 6.;
static constexpr Real c7o1 = 7.;
static constexpr Real c8o1 = 8.;
static constexpr Real c9o1 = 9.;
static constexpr Real c10o1 = 10.;
static constexpr Real c11o1 = 11.;
static constexpr Real c12o1 = 12.;
static constexpr Real c13o1 = 13.;
static constexpr Real c14o1 = 14.;
static constexpr Real c15o1 = 15.;
static constexpr Real c16o1 = 16.;
static constexpr Real c17o1 = 17.;
static constexpr Real c18o1 = 18.;
static constexpr Real c21o1 = 21.;
static constexpr Real c24o1 = 24.;
static constexpr Real c25o1 = 25.;
static constexpr Real c26o1 = 26.;
static constexpr Real c27o1 = 27.;
static constexpr Real c28o1 = 28.;
static constexpr Real c29o1 = 29.;
static constexpr Real c30o1 = 30.;
static constexpr Real c32o1 = 32.;
static constexpr Real c33o1 = 33.;
static constexpr Real c34o1 = 34.;
static constexpr Real c36o1 = 36.;
static constexpr Real c40o1 = 40.;
static constexpr Real c42o1 = 42.;
static constexpr Real c46o1 = 46.;
static constexpr Real c48o1 = 48.;
static constexpr Real c50o1 = 50.;
static constexpr Real c52o1 = 52.;
static constexpr Real c54o1 = 54.;
static constexpr Real c56o1 = 56.;
static constexpr Real c64o1 = 64.;
static constexpr Real c66o1 = 66.;
static constexpr Real c68o1 = 68.;
static constexpr Real c69o1 = 69.;
static constexpr Real c72o1 = 72.;
static constexpr Real c84o1 = 84.;
static constexpr Real c88o1 = 88.;
static constexpr Real c96o1 = 96.;
static constexpr Real c100o1 = 100.0;
static constexpr Real c130o1 = 130.0;
static constexpr Real c152o1 = 152.0;
static constexpr Real c166o1 = 166.0;
static constexpr Real c195o1 = 195.0;
static constexpr Real c216o1 = 216.0;
static constexpr Real c264o1 = 264.0;
static constexpr Real c290o1 = 290.0;
static constexpr Real c367o1 = 367.0;

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void init_fluid(int i, int j, int k, amrex::Array4<amrex::Real> const& fold,
                amrex::Array4<amrex::Real> const& fnew) {
    for (int q = 0; q < Q; q++) {
        fold(i, j, k, q) = feqQian(rho0, {0.0, 0.0, 0.0}, q);
        fnew(i, j, k, q) = fold(i, j, k, q);
    }
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void init_fluid_channel(int i, int j, int k, amrex::Array4<amrex::Real> const& fold,
                        amrex::Array4<amrex::Real> const& fnew, amrex::IntVect const& hi, amrex::Real dx, amrex::RandomEngine const& rd) {
    amrex::Real ux, uy, uz;

    int NH_real = static_cast<int>(NH / dx); // 换算到lev0还是当前lev

    // 获取随机数
    amrex::Real randVal = amrex::Random(rd);  // 0-1
    amrex::Real randCentered = randVal - 0.5; // [-0.5, 0.5]
    // 将随机噪声幅值降低，避免高波数白噪声主导（方案A：以低波数模态为主，随机为辅）
    constexpr amrex::Real noiseScale = 0.05; // 相对 Uc 的比例
    amrex::Real randPert = noiseScale * Uc * randCentered;
    amrex::Real Kappa = 0.39; // 对数律参数
    amrex::Real B = 4.7;

    if (j == NH_real) {
        ux = Uc;
        uy = 0.0;
        uz = 0.0;
    } else if (j > NH_real) {
        amrex::Real yPlus = static_cast<amrex::Real>(hi[1] - j) * Ut / mv_0;

        if (yPlus <= 11.6) {
            ux = randPert + Ut * yPlus;
            uy = randPert;
            uz = randPert;
        } else {
            ux = randPert + Ut * (log(yPlus) / Kappa + B);
            uy = randPert;
            uz = randPert;
        }
    } else if (j < NH_real) {
        amrex::Real yPlus = static_cast<amrex::Real>(j) * Ut / mv_0;

        if (yPlus <= 11.6) {
            ux = randPert + Ut * yPlus;
            uy = randPert;
            uz = randPert;
        } else {
            ux = randPert + Ut * (log(yPlus) / Kappa + B);
            uy = randPert;
            uz = randPert;
        }
    }

    // 方案A：在基流之上叠加低波数多模态扰动（流/跨向低模态），增强触发转捩能力
    // 归一化坐标（按全域尺寸 NX/NZ；该常量在 D3Q19.H 中定义）
    amrex::Real phiX = static_cast<amrex::Real>(2.0 * PI) * static_cast<amrex::Real>(i) / static_cast<amrex::Real>(NX);
    amrex::Real phiZ = static_cast<amrex::Real>(2.0 * PI) * static_cast<amrex::Real>(k) / static_cast<amrex::Real>(NZ);

    // y 方向窗口（如需近壁强化可改为高斯/顶帽窗；这里先用全域 w=1.0）
    amrex::Real w_y = 1.0;

    // 扰动幅值（相对中心速度 Uc）
    amrex::Real epsx = 0.05 * Uc; // 主导流向
    amrex::Real epsy = 0.03 * Uc;
    amrex::Real epsz = 0.03 * Uc;

    // 低波数多模态（含少量二次谐波），零均值组合
    amrex::Real sx1 = std::sin(phiX);
    amrex::Real cx1 = std::cos(phiX);
    amrex::Real sz1 = std::sin(phiZ);
    amrex::Real cz1 = std::cos(phiZ);
    amrex::Real sx2 = std::sin(2.0 * phiX);
    amrex::Real cz2 = std::cos(2.0 * phiZ);

    amrex::Real dux = epsx * w_y * (sx1 * cz1 + 0.5 * sx2 * cz1 + 0.5 * sx1 * cz2);
    amrex::Real duy = epsy * w_y * (cx1 * sz1);
    amrex::Real duz = epsz * w_y * (-sx1 * sz1);

    ux += dux;
    uy += duy;
    uz += duz;

    // 速度限幅，避免 LBM 在初始化阶段超马赫（保守取 0.1*cs）
    amrex::Real umax = 0.1 * cs;
    amrex::Real umag2 = ux * ux + uy * uy + uz * uz;
    if (umag2 > (umax * umax)) {
        amrex::Real scale = umax / std::sqrt(umag2);
        ux *= scale;
        uy *= scale;
        uz *= scale;
    }

    for (int q = 0; q < Q; q++) {
        fold(i, j, k, q) = feqQian(rho0, {ux, uy, uz}, q);
        fnew(i, j, k, q) = fold(i, j, k, q);
    }
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void compute_macro(int i, int j, int k, amrex::Array4<amrex::Real> const& fold,
                   amrex::Array4<amrex::Real> const& rho, amrex::Array4<amrex::Real> const& u) {
    Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;

    for (int q = 0; q < Q; q++) {
        ft = fold(i, j, k, q);
        rhot += ft;
        uxt += ft * e[q][0];
        uyt += ft * e[q][1];
        uzt += ft * e[q][2];
    }

    u(i, j, k, 0) = uxt / rhot;
    u(i, j, k, 1) = uyt / rhot;
    u(i, j, k, 2) = uzt / rhot;
    rho(i, j, k, 0) = rhot;
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void compute_vorticity(int i, int j, int k, amrex::Array4<amrex::Real> const& u,
                       amrex::Array4<amrex::Real> const& vort, amrex::Real dt) {
    Real u_x = (u(i + 1, j, k, 0) - u(i - 1, j, k, 0)) / (2 * dt);
    Real u_y = (u(i, j + 1, k, 0) - u(i, j - 1, k, 0)) / (2 * dt);
    Real u_z = (u(i, j, k + 1, 0) - u(i, j, k - 1, 0)) / (2 * dt);
    Real v_x = (u(i + 1, j, k, 1) - u(i - 1, j, k, 1)) / (2 * dt);
    Real v_y = (u(i, j + 1, k, 1) - u(i, j - 1, k, 1)) / (2 * dt);
    Real v_z = (u(i, j, k + 1, 1) - u(i, j, k - 1, 1)) / (2 * dt);
    Real w_x = (u(i + 1, j, k, 2) - u(i - 1, j, k, 2)) / (2 * dt);
    Real w_y = (u(i, j + 1, k, 2) - u(i, j - 1, k, 2)) / (2 * dt);
    Real w_z = (u(i, j, k + 1, 2) - u(i, j, k - 1, 2)) / (2 * dt);

    Real WX = w_y - v_z;
    Real WY = u_z - w_x;
    Real WZ = v_x - u_y;

    Real Q_criterion = -0.5 * (u_x * u_x + v_y * v_y + w_z * w_z) - u_y * v_x - u_z * w_x - v_z * w_y;
    Real Vorticity = sqrt(WX * WX + WY * WY + WZ * WZ);

    vort(i, j, k, 0) = Q_criterion;
    vort(i, j, k, 1) = Vorticity;
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void compute_shear(int i, int j, int k, amrex::Array4<amrex::Real> const& u,
                   amrex::Array4<amrex::Real> const& shear, amrex::Real dt) {
    Real SS, Q_CSM, E_CSM, F_CSM, C_CSM;

    Real u_x = (u(i + 1, j, k, 0) - u(i - 1, j, k, 0)) / (2 * dt);
    Real u_y = (u(i, j + 1, k, 0) - u(i, j - 1, k, 0)) / (2 * dt);
    Real u_z = (u(i, j, k + 1, 0) - u(i, j, k - 1, 0)) / (2 * dt);
    Real v_x = (u(i + 1, j, k, 1) - u(i - 1, j, k, 1)) / (2 * dt);
    Real v_y = (u(i, j + 1, k, 1) - u(i, j - 1, k, 1)) / (2 * dt);
    Real v_z = (u(i, j, k + 1, 1) - u(i, j, k - 1, 1)) / (2 * dt);
    Real w_x = (u(i + 1, j, k, 2) - u(i - 1, j, k, 2)) / (2 * dt);
    Real w_y = (u(i, j + 1, k, 2) - u(i, j - 1, k, 2)) / (2 * dt);
    Real w_z = (u(i, j, k + 1, 2) - u(i, j, k - 1, 2)) / (2 * dt);

    Real XX = u_x;
    Real YY = v_y;
    Real ZZ = w_z;
    Real XY = 0.5 * (u_y + v_x);
    Real XZ = 0.5 * (u_z + w_x);
    Real YZ = 0.5 * (v_z + w_y);

    SS = 2.0 * (XX * XX + XY * XY + XZ * XZ +
                XY * XY + YY * YY + YZ * YZ +
                XZ * XZ + YZ * YZ + ZZ * ZZ);

    Q_CSM = (-0.5) * (u_x * u_x + u_y * v_x + u_z * w_x +
                      v_x * u_y + v_y * v_y + v_z * w_y +
                      w_x * u_z + w_y * v_z + w_z * w_z);

    E_CSM = 0.5 * (u_x * u_x + v_x * v_x + w_x * w_x +
                   u_y * u_y + v_y * v_y + w_y * w_y +
                   u_z * u_z + v_z * v_z + w_z * w_z);
    if (E_CSM != 0.0)
        F_CSM = Q_CSM / E_CSM;
    else
        F_CSM = 0.0;
    if (F_CSM < (-1.0))
        F_CSM = -1.0;
    if (F_CSM > (1.0))
        F_CSM = 1.0;
    C_CSM = 1.0 / 22.0 * pow(fabs(F_CSM), 1.5) * (1.0 - F_CSM);

    shear(i, j, k, 0) = C_CSM * dt * dt * sqrt(SS); // 不知道这样直接从三维改到二维对不对
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void compute_shear2(int i, int j, int k, amrex::Array4<amrex::Real> const& u,
                    amrex::Array4<amrex::Real> const& shear, amrex::Real dt) {
    Real SS, Q_CSM, E_CSM, F_CSM, C_CSM;

    Real u_x = (u(i + 1, j, k, 0) - u(i - 1, j, k, 0)) / (2 * dt);
    Real u_y = (u(i, j + 1, k, 0) - u(i, j - 1, k, 0)) / (2 * dt);
    Real u_z = (u(i, j, k + 1, 0) - u(i, j, k - 1, 0)) / (2 * dt);
    Real v_x = (u(i + 1, j, k, 1) - u(i - 1, j, k, 1)) / (2 * dt);
    Real v_y = (u(i, j + 1, k, 1) - u(i, j - 1, k, 1)) / (2 * dt);
    Real v_z = (u(i, j, k + 1, 1) - u(i, j, k - 1, 1)) / (2 * dt);
    Real w_x = (u(i + 1, j, k, 2) - u(i - 1, j, k, 2)) / (2 * dt);
    Real w_y = (u(i, j + 1, k, 2) - u(i, j - 1, k, 2)) / (2 * dt);
    Real w_z = (u(i, j, k + 1, 2) - u(i, j, k - 1, 2)) / (2 * dt);

    Real XX = u_x;
    Real YY = v_y;
    Real ZZ = w_z;
    Real XY = 0.5 * (u_y + v_x);
    Real XZ = 0.5 * (u_z + w_x);
    Real YZ = 0.5 * (v_z + w_y);

    Real Smag = sqrt(XX * XX + XY * XY + XZ * XZ +
                     XY * XY + YY * YY + YZ * YZ +
                     XZ * XZ + YZ * YZ + ZZ * ZZ);

    shear(i, j, k, 0) = 0.01 * dt * dt * Smag; // 不知道这样直接从三维改到二维对不对
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void fill_boundary(int i, int j, int k, amrex::Array4<amrex::Real> const& fold,
                   amrex::Array4<amrex::Real> const& fnew, amrex::IntVect const& hi) {

    if (i == 0) {
        int x = i;
        int y = j;
        int z = k;

        int x1 = x + 1;
        int y1 = y;
        int z1 = z;

        Real uxt1 = 0.0, uyt1 = 0.0, uzt1 = 0.0, rhot1 = 0.0, ft1 = 0.0;
        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0;

        for (int q = 0; q < Q; q++) {
            ft1 = fold(x1, y1, z1, q);
            rhot1 += ft1;
            uxt1 += ft1 * e[q][0];
            uyt1 += ft1 * e[q][1];
            uzt1 += ft1 * e[q][2];
        }

        uxt1 /= rhot1;
        uyt1 /= rhot1;
        uzt1 /= rhot1;

        uxt = 0.0;
        uyt = 0.0;
        uzt = 0.0;

        // uxt = uxt1;
        // uyt = uyt1;
        // uzt = uzt1;

        rhot = rhot1;

        for (int q = 0; q < Q; q++) {
            fnew(x, y, z, q) = feqQian(rhot, {uxt, uyt, uzt}, q) + fold(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1, uzt1}, q);
        }
    }

    if (i == hi[0]) // 右边界
    {
        int x = i;
        int y = j;
        int z = k;

        int x1 = x - 1;
        int y1 = y;
        int z1 = z;

        Real uxt1 = 0.0, uyt1 = 0.0, uzt1 = 0.0, rhot1 = 0.0, ft1 = 0.0;
        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0;

        for (int q = 0; q < Q; q++) {
            ft1 = fold(x1, y1, z1, q);
            rhot1 += ft1;
            uxt1 += ft1 * e[q][0];
            uyt1 += ft1 * e[q][1];
            uzt1 += ft1 * e[q][2];
        }

        uxt1 /= rhot1;
        uyt1 /= rhot1;
        uzt1 /= rhot1;

        uxt = 0.0;
        uyt = 0.0;
        uzt = 0.0;

        // uxt = uxt1;
        // uyt = uyt1;
        // uzt = uzt1;

        rhot = rhot1;

        for (int q = 0; q < Q; q++) {
            fnew(x, y, z, q) = feqQian(rhot, {uxt, uyt, uzt}, q) + fold(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1, uzt1}, q);
        }
    }

    if (j == 0) {
        int x = i;
        int y = j;
        int z = k;

        int x1 = x;
        int y1 = y + 1;
        int z1 = z;

        Real uxt1 = 0.0, uyt1 = 0.0, uzt1 = 0.0, rhot1 = 0.0, ft1 = 0.0;
        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0;

        for (int q = 0; q < Q; q++) {
            ft1 = fold(x1, y1, z1, q);
            rhot1 += ft1;
            uxt1 += ft1 * e[q][0];
            uyt1 += ft1 * e[q][1];
            uzt1 += ft1 * e[q][2];
        }

        uxt1 /= rhot1;
        uyt1 /= rhot1;
        uzt1 /= rhot1;

        uxt = 0.0;
        uyt = 0.0;
        uzt = 0.0;

        // uxt = uxt1;
        // uyt = uyt1;
        // uzt = uzt1;

        rhot = rhot1;

        for (int q = 0; q < Q; q++) {
            fnew(x, y, z, q) = feqQian(rhot, {uxt, uyt, uzt}, q) + fold(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1, uzt1}, q);
        }
    }

    if (j == hi[1]) {
        int x = i;
        int y = j;
        int z = k;

        int x1 = x;
        int y1 = y - 1;
        int z1 = z;

        Real uxt1 = 0.0, uyt1 = 0.0, uzt1 = 0.0, rhot1 = 0.0, ft1 = 0.0;
        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0;

        for (int q = 0; q < Q; q++) {
            ft1 = fold(x1, y1, z1, q);
            rhot1 += ft1;
            uxt1 += ft1 * e[q][0];
            uyt1 += ft1 * e[q][1];
            uzt1 += ft1 * e[q][2];
        }

        uxt1 /= rhot1;
        uyt1 /= rhot1;
        uzt1 /= rhot1;

        uxt = 0.0;
        uyt = 0.0;
        uzt = 0.0;

        // uxt = uxt1;
        // uyt = uyt1;
        // uzt = uzt1;

        rhot = rhot1;

        for (int q = 0; q < Q; q++) {
            fnew(x, y, z, q) = feqQian(rhot, {uxt, uyt, uzt}, q) + fold(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1, uzt1}, q);
        }
    }

    if (k == 0) {
        int x = i;
        int y = j;
        int z = k;

        int x1 = x;
        int y1 = y;
        int z1 = z + 1;

        Real uxt1 = 0.0, uyt1 = 0.0, uzt1 = 0.0, rhot1 = 0.0, ft1 = 0.0;
        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0;

        for (int q = 0; q < Q; q++) {
            ft1 = fold(x1, y1, z1, q);
            rhot1 += ft1;
            uxt1 += ft1 * e[q][0];
            uyt1 += ft1 * e[q][1];
            uzt1 += ft1 * e[q][2];
        }

        uxt1 /= rhot1;
        uyt1 /= rhot1;
        uzt1 /= rhot1;

        uxt = 0.0;
        uyt = 0.0;
        uzt = 0.0;

        // uxt = uxt1;
        // uyt = uyt1;
        // uzt = uzt1;

        rhot = rhot1;

        for (int q = 0; q < Q; q++) {
            fnew(x, y, z, q) = feqQian(rhot, {uxt, uyt, uzt}, q) + fold(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1, uzt1}, q);
        }
    }

    if (k == hi[2]) {
        int x = i;
        int y = j;
        int z = k;

        int x1 = x;
        int y1 = y;
        int z1 = z - 1;

        Real uxt1 = 0.0, uyt1 = 0.0, uzt1 = 0.0, rhot1 = 0.0, ft1 = 0.0;
        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0;

        for (int q = 0; q < Q; q++) {
            ft1 = fold(x1, y1, z1, q);
            rhot1 += ft1;
            uxt1 += ft1 * e[q][0];
            uyt1 += ft1 * e[q][1];
            uzt1 += ft1 * e[q][2];
        }

        uxt1 /= rhot1;
        uyt1 /= rhot1;
        uzt1 /= rhot1;

        uxt = 0.0;
        uyt = 0.0;
        uzt = 0.0;

        // uxt = uxt1;
        // uyt = uyt1;
        // uzt = uzt1;

        rhot = rhot1;

        for (int q = 0; q < Q; q++) {
            fnew(x, y, z, q) = feqQian(rhot, {uxt, uyt, uzt}, q) + fold(x1, y1, z1, q) - feqQian(rhot1, {uxt1, uyt1, uzt1}, q);
        }
    }

    if (i < 0 || i > hi[0] || j < 0 || j > hi[1] || k < 0 || k > hi[2]) {
        for (int q = 0; q < Q; q++) {
            fnew(i, j, k, q) = 0.0;
            fold(i, j, k, q) = 0.0;
        }
    }
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void collide(int i, int j, int k, amrex::Array4<amrex::Real> const& fold, amrex::Array4<amrex::Real> const& s,
             amrex::Array4<amrex::Real> const& Ft, amrex::Real tau_lev, amrex::Real dt, amrex::IntVect const& hi) {
    Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;
    Real Ftx = 0.0, Fty = 0.0, Ftz = 0.0;

    s(i, j, k, 0) = 0.0;

    Real tauStar = tau_lev + 3.0 * s(i, j, k, 0) / cs2 / dt;
    Real omega = 1.0 / tauStar;

    for (int q = 0; q < Q; q++) {
        ft = fold(i, j, k, q);
        rhot += ft;
        uxt += ft * e[q][0];
        uyt += ft * e[q][1];
        uzt += ft * e[q][2];
    }

    Ftx = Ft(i, j, k, 0);
    Fty = Ft(i, j, k, 1);
    Ftz = Ft(i, j, k, 2);

    uxt /= rhot;
    uyt /= rhot;
    uzt /= rhot;

    uxt += 0.5 * dt * Ftx / (rhot);
    uyt += 0.5 * dt * Fty / (rhot);
    uzt += 0.5 * dt * Ftz / (rhot);

    // if((i >= 1) && (i < hi[0]) && (j >= 1) && (j < hi[1]))
    // {
    for (int q = 0; q < Q; q++) {
        Real forceEx = (1 - 0.5 * omega) * dt * forceGuo({uxt, uyt, uzt}, {Ftx, Fty, Ftz}, q); // * cs2;
        fold(i, j, k, q) = (1 - omega) * fold(i, j, k, q) + omega * feqQian(rhot, {uxt, uyt, uzt}, q) + forceEx;
    }
    // }
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void collide_cumulant(int i, int j, int k, amrex::Array4<amrex::Real> const& fold, amrex::Array4<amrex::Real> const& s,
                      amrex::Array4<amrex::Real> const& Ft, amrex::Real tau_lev, amrex::Real dt, amrex::IntVect const& hi) {
    Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;
    Real Ftx = 0.0, Fty = 0.0, Ftz = 0.0;

    Real omega = 1.0 / tau_lev;

    for (int q = 0; q < Q; q++) {
        ft = fold(i, j, k, q);
        rhot += ft;
        uxt += ft * e[q][0];
        uyt += ft * e[q][1];
        uzt += ft * e[q][2];
    }

    // Ftx = Ft(i, j, k, 0);
    // Fty = Ft(i, j, k, 1);
    // Ftz = Ft(i, j, k, 2);

    Ftx = mv_0 * mv_0 * Ret * Ret / (NH * NH * NH);
    Fty = 0.0;
    Ftz = 0.0;

    uxt /= rhot;
    uyt /= rhot;
    uzt /= rhot;

    uxt += 0.5 * dt * Ftx / (rhot);
    uyt += 0.5 * dt * Fty / (rhot);
    uzt += 0.5 * dt * Ftz / (rhot);

    // 松弛因子
    const Real omega1 = omega;
    const Real omega2 = 1.0;
    const Real omega3 = 1.0;
    const Real omega4 = 1.0;
    const Real omega5 = 1.0;
    const Real omega6 = 1.0;
    const Real omega7 = 1.0;
    const Real omega8 = 1.0;
    const Real omega9 = 1.0;
    const Real omega10 = 1.0;

    const Real omega_trm1((1.0) - omega1);
    const Real omega_trm2((1.0) - omega2);
    const Real omega_trm3((1.0) - omega3);
    const Real omega_trm4((1.0) - omega4);
    const Real omega_trm5((1.0) - omega5);
    const Real omega_trm6((1.0) - omega6);
    const Real omega_trm7((1.0) - omega7);
    const Real omega_trm8((1.0) - omega8);
    const Real omega_trm9((1.0) - omega9);
    const Real omega_trm10((1.0) - omega10);

    // 取出分布函数
    const Real vC = fold(i, j, k, 0);
    const Real vN = fold(i, j, k, 1);
    const Real vS = fold(i, j, k, 2);
    const Real vW = fold(i, j, k, 3);
    const Real vE = fold(i, j, k, 4);
    const Real vT = fold(i, j, k, 5);
    const Real vB = fold(i, j, k, 6);
    const Real vNW = fold(i, j, k, 7);
    const Real vNE = fold(i, j, k, 8);
    const Real vSW = fold(i, j, k, 9);
    const Real vSE = fold(i, j, k, 10);
    const Real vTN = fold(i, j, k, 11);
    const Real vTS = fold(i, j, k, 12);
    const Real vTW = fold(i, j, k, 13);
    const Real vTE = fold(i, j, k, 14);
    const Real vBN = fold(i, j, k, 15);
    const Real vBS = fold(i, j, k, 16);
    const Real vBW = fold(i, j, k, 17);
    const Real vBE = fold(i, j, k, 18);
    const Real vTNE = fold(i, j, k, 19);
    const Real vTNW = fold(i, j, k, 20);
    const Real vTSE = fold(i, j, k, 21);
    const Real vTSW = fold(i, j, k, 22);
    const Real vBNE = fold(i, j, k, 23);
    const Real vBNW = fold(i, j, k, 24);
    const Real vBSE = fold(i, j, k, 25);
    const Real vBSW = fold(i, j, k, 26);

    // 分布函数转换到中心距空间
    const Real velX = uxt;
    const Real velY = uyt;
    const Real velZ = uzt;
    const Real velXX = velX * velX;
    const Real velYY = velY * velY;
    const Real velZZ = velZ * velZ;

    const Real velz_term = (-1.0) * uzt;
    const Real sqr_velz_term = velz_term * velz_term;
    const Real oneminus_velz = (1.0) - uzt;
    const Real sqr_oneminus_velz = oneminus_velz * oneminus_velz;
    const Real negoneminus_velz = (-1.0) - uzt;
    const Real sqr_negoneminus_velz = negoneminus_velz * negoneminus_velz;

    const Real k_00_0 = vBSW + vSW + vTSW;
    const Real k_00_1 = vBSW * negoneminus_velz + vSW * velz_term + vTSW * oneminus_velz;
    const Real k_00_2 = vBSW * sqr_negoneminus_velz + vSW * sqr_velz_term + vTSW * sqr_oneminus_velz;
    const Real k_01_0 = vBW + vW + vTW;
    const Real k_01_1 = vBW * negoneminus_velz + vW * velz_term + vTW * oneminus_velz;
    const Real k_01_2 = vBW * sqr_negoneminus_velz + vW * sqr_velz_term + vTW * sqr_oneminus_velz;
    const Real k_02_0 = vBNW + vNW + vTNW;
    const Real k_02_1 = vBNW * negoneminus_velz + vNW * velz_term + vTNW * oneminus_velz;
    const Real k_02_2 = vBNW * sqr_negoneminus_velz + vNW * sqr_velz_term + vTNW * sqr_oneminus_velz;
    const Real k_10_0 = vBS + vS + vTS;
    const Real k_10_1 = vBS * negoneminus_velz + vS * velz_term + vTS * oneminus_velz;
    const Real k_10_2 = vBS * sqr_negoneminus_velz + vS * sqr_velz_term + vTS * sqr_oneminus_velz;
    const Real k_11_0 = vB + vC + vT;
    const Real k_11_1 = vB * negoneminus_velz + vC * velz_term + vT * oneminus_velz;
    const Real k_11_2 = vB * sqr_negoneminus_velz + vC * sqr_velz_term + vT * sqr_oneminus_velz;
    const Real k_12_0 = vBN + vN + vTN;
    const Real k_12_1 = vBN * negoneminus_velz + vN * velz_term + vTN * oneminus_velz;
    const Real k_12_2 = vBN * sqr_negoneminus_velz + vN * sqr_velz_term + vTN * sqr_oneminus_velz;
    const Real k_20_0 = vBSE + vSE + vTSE;
    const Real k_20_1 = vBSE * negoneminus_velz + vSE * velz_term + vTSE * oneminus_velz;
    const Real k_20_2 = vBSE * sqr_negoneminus_velz + vSE * sqr_velz_term + vTSE * sqr_oneminus_velz;
    const Real k_21_0 = vBE + vE + vTE;
    const Real k_21_1 = vBE * negoneminus_velz + vE * velz_term + vTE * oneminus_velz;
    const Real k_21_2 = vBE * sqr_negoneminus_velz + vE * sqr_velz_term + vTE * sqr_oneminus_velz;
    const Real k_22_0 = vBNE + vNE + vTNE;
    const Real k_22_1 = vBNE * negoneminus_velz + vNE * velz_term + vTNE * oneminus_velz;
    const Real k_22_2 = vBNE * sqr_negoneminus_velz + vNE * sqr_velz_term + vTNE * sqr_oneminus_velz;

    const Real vely_term = Real(-1.0) * velY;
    const Real sqr_vely_term = vely_term * vely_term;
    const Real oneminus_vely = Real(1.0) - velY;
    const Real sqr_oneminus_vely = oneminus_vely * oneminus_vely;
    const Real negoneminus_vely = Real(-1.0) - velY;
    const Real sqr_negoneminus_vely = negoneminus_vely * negoneminus_vely;

    const Real k_0_00 = k_00_0 + k_01_0 + k_02_0;
    const Real k_0_10 = k_00_0 * negoneminus_vely + k_01_0 * vely_term + k_02_0 * oneminus_vely;
    const Real k_0_20 = k_00_0 * sqr_negoneminus_vely + k_01_0 * sqr_vely_term + k_02_0 * sqr_oneminus_vely;
    const Real k_0_01 = k_00_1 + k_01_1 + k_02_1;
    const Real k_0_11 = k_00_1 * negoneminus_vely + k_01_1 * vely_term + k_02_1 * oneminus_vely;
    const Real k_0_21 = k_00_1 * sqr_negoneminus_vely + k_01_1 * sqr_vely_term + k_02_1 * sqr_oneminus_vely;
    const Real k_0_02 = k_00_2 + k_01_2 + k_02_2;
    const Real k_0_12 = k_00_2 * negoneminus_vely + k_01_2 * vely_term + k_02_2 * oneminus_vely;
    const Real k_0_22 = k_00_2 * sqr_negoneminus_vely + k_01_2 * sqr_vely_term + k_02_2 * sqr_oneminus_vely;
    const Real k_1_00 = k_10_0 + k_11_0 + k_12_0;
    const Real k_1_10 = k_10_0 * negoneminus_vely + k_11_0 * vely_term + k_12_0 * oneminus_vely;
    const Real k_1_20 = k_10_0 * sqr_negoneminus_vely + k_11_0 * sqr_vely_term + k_12_0 * sqr_oneminus_vely;
    const Real k_1_01 = k_10_1 + k_11_1 + k_12_1;
    const Real k_1_11 = k_10_1 * negoneminus_vely + k_11_1 * vely_term + k_12_1 * oneminus_vely;
    const Real k_1_21 = k_10_1 * sqr_negoneminus_vely + k_11_1 * sqr_vely_term + k_12_1 * sqr_oneminus_vely;
    const Real k_1_02 = k_10_2 + k_11_2 + k_12_2;
    const Real k_1_12 = k_10_2 * negoneminus_vely + k_11_2 * vely_term + k_12_2 * oneminus_vely;
    const Real k_1_22 = k_10_2 * sqr_negoneminus_vely + k_11_2 * sqr_vely_term + k_12_2 * sqr_oneminus_vely;
    const Real k_2_00 = k_20_0 + k_21_0 + k_22_0;
    const Real k_2_10 = k_20_0 * negoneminus_vely + k_21_0 * vely_term + k_22_0 * oneminus_vely;
    const Real k_2_20 = k_20_0 * sqr_negoneminus_vely + k_21_0 * sqr_vely_term + k_22_0 * sqr_oneminus_vely;
    const Real k_2_01 = k_20_1 + k_21_1 + k_22_1;
    const Real k_2_11 = k_20_1 * negoneminus_vely + k_21_1 * vely_term + k_22_1 * oneminus_vely;
    const Real k_2_21 = k_20_1 * sqr_negoneminus_vely + k_21_1 * sqr_vely_term + k_22_1 * sqr_oneminus_vely;
    const Real k_2_02 = k_20_2 + k_21_2 + k_22_2;
    const Real k_2_12 = k_20_2 * negoneminus_vely + k_21_2 * vely_term + k_22_2 * oneminus_vely;
    const Real k_2_22 = k_20_2 * sqr_negoneminus_vely + k_21_2 * sqr_vely_term + k_22_2 * sqr_oneminus_vely;

    const Real velx_term = Real(-1.0) * velX;
    const Real sqr_velx_term = velx_term * velx_term;
    const Real oneminus_velx = Real(1.0) - velX;
    const Real sqr_oneminus_velx = oneminus_velx * oneminus_velx;
    const Real negoneminus_velx = Real(-1.0) - velX;
    const Real sqr_negoneminus_velx = negoneminus_velx * negoneminus_velx;

    const Real k_000 = k_0_00 + k_1_00 + k_2_00;
    const Real k_100 = k_0_00 * negoneminus_velx + k_1_00 * velx_term + k_2_00 * oneminus_velx;
    const Real k_200 = k_0_00 * sqr_negoneminus_velx + k_1_00 * sqr_velx_term + k_2_00 * sqr_oneminus_velx;
    const Real k_001 = k_0_01 + k_1_01 + k_2_01;
    const Real k_101 = k_0_01 * negoneminus_velx + k_1_01 * velx_term + k_2_01 * oneminus_velx;
    const Real k_201 = k_0_01 * sqr_negoneminus_velx + k_1_01 * sqr_velx_term + k_2_01 * sqr_oneminus_velx;
    const Real k_002 = k_0_02 + k_1_02 + k_2_02;
    const Real k_102 = k_0_02 * negoneminus_velx + k_1_02 * velx_term + k_2_02 * oneminus_velx;
    const Real k_202 = k_0_02 * sqr_negoneminus_velx + k_1_02 * sqr_velx_term + k_2_02 * sqr_oneminus_velx;
    const Real k_010 = k_0_10 + k_1_10 + k_2_10;
    const Real k_110 = k_0_10 * negoneminus_velx + k_1_10 * velx_term + k_2_10 * oneminus_velx;
    const Real k_210 = k_0_10 * sqr_negoneminus_velx + k_1_10 * sqr_velx_term + k_2_10 * sqr_oneminus_velx;
    const Real k_011 = k_0_11 + k_1_11 + k_2_11;
    const Real k_111 = k_0_11 * negoneminus_velx + k_1_11 * velx_term + k_2_11 * oneminus_velx;
    const Real k_211 = k_0_11 * sqr_negoneminus_velx + k_1_11 * sqr_velx_term + k_2_11 * sqr_oneminus_velx;
    const Real k_012 = k_0_12 + k_1_12 + k_2_12;
    const Real k_112 = k_0_12 * negoneminus_velx + k_1_12 * velx_term + k_2_12 * oneminus_velx;
    const Real k_212 = k_0_12 * sqr_negoneminus_velx + k_1_12 * sqr_velx_term + k_2_12 * sqr_oneminus_velx;
    const Real k_020 = k_0_20 + k_1_20 + k_2_20;
    const Real k_120 = k_0_20 * negoneminus_velx + k_1_20 * velx_term + k_2_20 * oneminus_velx;
    const Real k_220 = k_0_20 * sqr_negoneminus_velx + k_1_20 * sqr_velx_term + k_2_20 * sqr_oneminus_velx;
    const Real k_021 = k_0_21 + k_1_21 + k_2_21;
    const Real k_121 = k_0_21 * negoneminus_velx + k_1_21 * velx_term + k_2_21 * oneminus_velx;
    const Real k_221 = k_0_21 * sqr_negoneminus_velx + k_1_21 * sqr_velx_term + k_2_21 * sqr_oneminus_velx;
    const Real k_022 = k_0_22 + k_1_22 + k_2_22;
    const Real k_122 = k_0_22 * negoneminus_velx + k_1_22 * velx_term + k_2_22 * oneminus_velx;
    const Real k_222 = k_0_22 * sqr_negoneminus_velx + k_1_22 * sqr_velx_term + k_2_22 * sqr_oneminus_velx;

    // 从中心距空间转移到积累量空间
    const Real updated_rho = rhot;
    const Real rho_inv = 1.0 / updated_rho;
    const Real sqr_k_110 = k_110 * k_110;
    const Real sqr_k_101 = k_101 * k_101;
    const Real sqr_k_011 = k_011 * k_011;
    const Real sqr_k_111 = k_111 * k_111;
    const Real sqr_rho_inv = rho_inv * rho_inv;

    const Real C_000 = k_000;
    const Real C_001 = k_001;
    const Real C_002 = k_002;
    const Real C_010 = k_010;
    const Real C_011 = k_011;
    const Real C_012 = k_012;
    const Real C_020 = k_020;
    const Real C_021 = k_021;
    const Real C_022 = k_022 - (k_020 * k_002 + 2.0 * sqr_k_011) * rho_inv;
    const Real C_100 = k_100;
    const Real C_101 = k_101;
    const Real C_102 = k_102;
    const Real C_110 = k_110;
    const Real C_111 = k_111;
    const Real C_112 = k_112 - (k_002 * k_110 + 2.0 * k_101 * k_011) * rho_inv;
    const Real C_120 = k_120;
    const Real C_121 = k_121 - (k_020 * k_101 + 2.0 * k_011 * k_110) * rho_inv;
    const Real C_122 = k_122 - (k_002 * k_120 + k_020 * k_120 + 4.0 * k_011 * k_111 + 2.0 * (k_101 * k_021 + k_110 * k_012)) * rho_inv;
    const Real C_200 = k_200;
    const Real C_201 = k_201;
    const Real C_202 = k_202 - (k_002 * k_200 + 2.0 * sqr_k_101) * rho_inv;
    const Real C_210 = k_210;
    const Real C_211 = k_211 - (k_200 * k_011 + 2.0 * k_110 * k_101) * rho_inv;
    const Real C_212 = k_212 - (k_002 * k_210 + k_200 * k_210 + 4.0 * k_101 * k_111 + (2.0) * (k_011 * k_201 + k_110 * k_102)) * rho_inv;
    const Real C_220 = k_220 - (k_200 * k_020 + (2.0) * sqr_k_110) * rho_inv;
    const Real C_221 = k_221 - (k_020 * k_201 + k_200 * k_201 + (4.0) * k_110 * k_111 + (2.0) * (k_011 * k_210 + k_101 * k_120)) * rho_inv;
    const Real C_222 = k_222 - ((4.0) * sqr_k_111 + k_200 * k_022 + k_020 * k_202 + k_002 * k_220 + (4.0) * (k_011 * k_211 + k_101 * k_121 + k_110 * k_112) + (2.0) * (k_120 * k_102 + k_210 * k_012 + k_201 * k_021)) * rho_inv + ((16.0) * k_110 * k_101 * k_011 + (4.0) * (k_020 * sqr_k_101 + k_200 * sqr_k_011 + k_002 * sqr_k_110) + (2.0) * k_200 * k_020 * k_002) * sqr_rho_inv;

    // 在积累量空间碰撞
    const Real CS_000 = C_000;
    const Real CS_100 = C_100;
    const Real CS_010 = C_010;
    const Real CS_001 = C_001;
    const Real CS_110 = omega_trm1 * C_110;
    const Real CS_101 = omega_trm1 * C_101;
    const Real CS_011 = omega_trm1 * C_011;

    const Real Dxux = (-0.5) * omega1 * rho_inv * ((2.0) * C_200 - C_020 - C_002) - (0.5) * omega2 * rho_inv * (C_200 + C_020 + C_002 - C_000);
    const Real Dyuy = Dxux + (1.5) * omega1 * rho_inv * (C_200 - C_020);
    const Real Dzuz = Dxux + (1.5) * omega1 * rho_inv * (C_200 - C_002);

    const Real CS_200__m__CS020 = (omega_trm1) * (C_200 - C_020) - (3.0) * updated_rho * ((1.0) - (0.5) * omega1) * (Dxux * velXX - Dyuy * velYY);
    const Real CS_200__m__CS002 = (omega_trm1) * (C_200 - C_002) - (3.0) * updated_rho * ((1.0) - (0.5) * omega1) * (Dxux * velXX - Dzuz * velZZ);
    const Real CS_200__p__CS020__p__CS_002 = omega2 * C_000 + (omega_trm2) * (C_200 + C_020 + C_002) - (3.0) * updated_rho * ((1.0) - (0.5) * omega2) * (Dxux * velXX + Dyuy * velYY + Dzuz * velZZ);
    const Real CS_200 = (CS_200__m__CS020 + CS_200__m__CS002 + CS_200__p__CS020__p__CS_002) / (3.0);
    const Real CS_020 = CS_200 - CS_200__m__CS020;
    const Real CS_002 = CS_200 - CS_200__m__CS002;

    const Real CS_120__p__CS_102 = (omega_trm3) * (C_120 + C_102);
    const Real CS_210__p__CS_012 = (omega_trm3) * (C_210 + C_012);
    const Real CS_201__p__CS_021 = (omega_trm3) * (C_201 + C_021);
    const Real CS_120__m__CS_102 = (omega_trm4) * (C_120 - C_102);
    const Real CS_210__m__CS_012 = (omega_trm4) * (C_210 - C_012);
    const Real CS_201__m__CS_021 = (omega_trm4) * (C_201 - C_021);

    const Real CS_120 = (0.5) * (CS_120__p__CS_102 + CS_120__m__CS_102);
    const Real CS_102 = (0.5) * (CS_120__p__CS_102 - CS_120__m__CS_102);
    const Real CS_012 = (0.5) * (CS_210__p__CS_012 - CS_210__m__CS_012);
    const Real CS_210 = (0.5) * (CS_210__p__CS_012 + CS_210__m__CS_012);
    const Real CS_201 = (0.5) * (CS_201__p__CS_021 + CS_201__m__CS_021);
    const Real CS_021 = (0.5) * (CS_201__p__CS_021 - CS_201__m__CS_021);
    const Real CS_111 = (omega_trm5)*C_111;

    const Real CS_220__m__2CS_202__p__CS_022 = (omega_trm6) * (C_220 - (2.0) * C_202 + C_022);
    const Real CS_220__p__CS_202__m__2CS_022 = (omega_trm6) * (C_220 + C_202 - (2.0) * C_022);
    const Real CS_220__p__CS_202__p__CS_022 = (omega_trm7) * (C_220 + C_202 + C_022);

    const Real CS_220 = (CS_220__m__2CS_202__p__CS_022 + CS_220__p__CS_202__m__2CS_022 + CS_220__p__CS_202__p__CS_022) / (3.0);
    const Real CS_202 = (CS_220__p__CS_202__p__CS_022 - CS_220__m__2CS_202__p__CS_022) / (3.0);
    const Real CS_022 = (CS_220__p__CS_202__p__CS_022 - CS_220__p__CS_202__m__2CS_022) / (3.0);

    const Real CS_211 = (omega_trm8)*C_211;
    const Real CS_121 = (omega_trm8)*C_121;
    const Real CS_112 = (omega_trm8)*C_112;
    const Real CS_221 = (omega_trm9)*C_221;
    const Real CS_212 = (omega_trm9)*C_212;
    const Real CS_122 = (omega_trm9)*C_122;
    const Real CS_222 = (omega_trm10)*C_222;

    // 从积累量空间转移回到中心距空间
    const Real KC_000 = CS_000;
    const Real KC_100 = -1.0 * k_100; // CS_100;//三个一阶矩可能要考虑外力的作用？
    const Real KC_200 = CS_200;
    const Real KC_001 = -1.0 * k_001; // CS_001;
    const Real KC_101 = CS_101;
    const Real KC_201 = CS_201;
    const Real KC_002 = CS_002;
    const Real KC_102 = CS_102;

    const Real sqr_KC_101 = KC_101 * KC_101;

    const Real KC_202 = CS_202 + (KC_002 * KC_200 + (2.0) * sqr_KC_101) * rho_inv;
    const Real KC_010 = -1.0 * k_010; // CS_010 ;
    const Real KC_110 = CS_110;
    const Real KC_210 = CS_210;
    const Real KC_011 = CS_011;
    const Real KC_111 = CS_111;
    const Real KC_211 = CS_211 + (KC_200 * KC_011 + (2.0) * KC_110 * KC_101) * rho_inv;
    const Real KC_012 = CS_012;
    const Real KC_112 = CS_112 + (KC_002 * KC_110 + (2.0) * KC_101 * KC_011) * rho_inv;
    const Real KC_212 = CS_212 + (KC_002 * KC_210 + KC_200 * KC_210 + (4.0) * KC_101 * KC_111 + (2.0) * (KC_011 * KC_201 + KC_110 * KC_102)) * rho_inv;
    const Real KC_020 = CS_020;
    const Real KC_120 = CS_120;

    const Real sqr_KC_110 = KC_110 * KC_110;

    const Real KC_220 = CS_220 + (KC_200 * KC_020 + (2.0) * sqr_KC_110) * rho_inv;
    const Real KC_021 = CS_021;
    const Real KC_121 = CS_121 + (KC_020 * KC_101 + (2.0) * KC_011 * KC_110) * rho_inv;
    const Real KC_221 = CS_221 + (KC_020 * KC_201 + KC_200 * KC_201 + (4.0) * KC_110 * KC_111 + (2.0) * (KC_011 * KC_210 + KC_101 * KC_120)) * rho_inv;

    const Real sqr_KC_011 = KC_011 * KC_011;

    const Real KC_022 = CS_022 + (KC_020 * KC_002 + (2.0) * sqr_KC_011) * rho_inv;
    const Real KC_122 = CS_122 + (KC_002 * KC_120 + KC_020 * KC_120 + (4.0) * KC_011 * KC_111 + (2.0) * (KC_101 * KC_021 + KC_110 * KC_012)) * rho_inv;

    const Real sqr_KC_111 = KC_111 * KC_111;

    const Real KC_222 = CS_222 + ((4.0) * sqr_KC_111 + KC_200 * KC_022 + KC_020 * KC_202 + KC_002 * KC_220 + (4.0) * (KC_011 * KC_211 + KC_101 * KC_121 + KC_110 * KC_112) + (2.0) * (KC_120 * KC_102 + KC_210 * KC_012 + KC_201 * KC_021)) * rho_inv - ((16.0) * KC_110 * KC_101 * KC_011 + (4.0) * (KC_020 * sqr_KC_101 + KC_200 * sqr_KC_011 + KC_002 * sqr_KC_110) + (2.0) * KC_200 * KC_020 * KC_002) * sqr_rho_inv;

    // 从中心距空间转移回到离散速度空间
    const Real oneminus_sqr_velx = (1.0) - velXX;
    const Real sqr_velx_plus_velX = velXX + velX;
    const Real sqr_velx_minus_velX = velXX - velX;
    const Real velx_term_plus = (2.0) * velX + (1.0);
    const Real velx_term_minus = (2.0) * velX - (1.0);

    const Real KC_1_00 = KC_000 * oneminus_sqr_velx - KC_100 * (2.0) * velX - KC_200;
    const Real KC_0_00 = (KC_000 * sqr_velx_minus_velX + KC_100 * velx_term_minus + KC_200) * (0.5);
    const Real KC_2_00 = (KC_000 * sqr_velx_plus_velX + KC_100 * velx_term_plus + KC_200) * (0.5);
    const Real KC_1_01 = KC_001 * oneminus_sqr_velx - KC_101 * (2.0) * velX - KC_201;
    const Real KC_0_01 = (KC_001 * sqr_velx_minus_velX + KC_101 * velx_term_minus + KC_201) * (0.5);
    const Real KC_2_01 = (KC_001 * sqr_velx_plus_velX + KC_101 * velx_term_plus + KC_201) * (0.5);
    const Real KC_1_02 = KC_002 * oneminus_sqr_velx - KC_102 * (2.0) * velX - KC_202;
    const Real KC_0_02 = (KC_002 * sqr_velx_minus_velX + KC_102 * velx_term_minus + KC_202) * (0.5);
    const Real KC_2_02 = (KC_002 * sqr_velx_plus_velX + KC_102 * velx_term_plus + KC_202) * (0.5);
    const Real KC_1_10 = KC_010 * oneminus_sqr_velx - KC_110 * (2.0) * velX - KC_210;
    const Real KC_0_10 = (KC_010 * sqr_velx_minus_velX + KC_110 * velx_term_minus + KC_210) * (0.5);
    const Real KC_2_10 = (KC_010 * sqr_velx_plus_velX + KC_110 * velx_term_plus + KC_210) * (0.5);
    const Real KC_1_11 = KC_011 * oneminus_sqr_velx - KC_111 * (2.0) * velX - KC_211;
    const Real KC_0_11 = (KC_011 * sqr_velx_minus_velX + KC_111 * velx_term_minus + KC_211) * (0.5);
    const Real KC_2_11 = (KC_011 * sqr_velx_plus_velX + KC_111 * velx_term_plus + KC_211) * (0.5);
    const Real KC_1_12 = KC_012 * oneminus_sqr_velx - KC_112 * (2.0) * velX - KC_212;
    const Real KC_0_12 = (KC_012 * sqr_velx_minus_velX + KC_112 * velx_term_minus + KC_212) * (0.5);
    const Real KC_2_12 = (KC_012 * sqr_velx_plus_velX + KC_112 * velx_term_plus + KC_212) * (0.5);
    const Real KC_1_20 = KC_020 * oneminus_sqr_velx - KC_120 * (2.0) * velX - KC_220;
    const Real KC_0_20 = (KC_020 * sqr_velx_minus_velX + KC_120 * velx_term_minus + KC_220) * (0.5);
    const Real KC_2_20 = (KC_020 * sqr_velx_plus_velX + KC_120 * velx_term_plus + KC_220) * (0.5);
    const Real KC_1_21 = KC_021 * oneminus_sqr_velx - KC_121 * (2.0) * velX - KC_221;
    const Real KC_0_21 = (KC_021 * sqr_velx_minus_velX + KC_121 * velx_term_minus + KC_221) * (0.5);
    const Real KC_2_21 = (KC_021 * sqr_velx_plus_velX + KC_121 * velx_term_plus + KC_221) * (0.5);
    const Real KC_1_22 = KC_022 * oneminus_sqr_velx - KC_122 * (2.0) * velX - KC_222;
    const Real KC_0_22 = (KC_022 * sqr_velx_minus_velX + KC_122 * velx_term_minus + KC_222) * (0.5);
    const Real KC_2_22 = (KC_022 * sqr_velx_plus_velX + KC_122 * velx_term_plus + KC_222) * (0.5);

    const Real oneminus_sqr_vely = (1.0) - velYY;
    const Real sqr_vely_plus_velY = velYY + velY;
    const Real sqr_vely_minus_velY = velYY - velY;
    const Real vely_term_plus = (2.0) * velY + (1.0);
    const Real vely_term_minus = (2.0) * velY - (1.0);

    const Real KC_01_0 = KC_0_00 * oneminus_sqr_vely - KC_0_10 * (2.0) * velY - KC_0_20;
    const Real KC_00_0 = (KC_0_00 * sqr_vely_minus_velY + KC_0_10 * vely_term_minus + KC_0_20) * (0.5);
    const Real KC_02_0 = (KC_0_00 * sqr_vely_plus_velY + KC_0_10 * vely_term_plus + KC_0_20) * (0.5);
    const Real KC_01_1 = KC_0_01 * oneminus_sqr_vely - KC_0_11 * (2.0) * velY - KC_0_21;
    const Real KC_00_1 = (KC_0_01 * sqr_vely_minus_velY + KC_0_11 * vely_term_minus + KC_0_21) * (0.5);
    const Real KC_02_1 = (KC_0_01 * sqr_vely_plus_velY + KC_0_11 * vely_term_plus + KC_0_21) * (0.5);
    const Real KC_01_2 = KC_0_02 * oneminus_sqr_vely - KC_0_12 * (2.0) * velY - KC_0_22;
    const Real KC_00_2 = (KC_0_02 * sqr_vely_minus_velY + KC_0_12 * vely_term_minus + KC_0_22) * (0.5);
    const Real KC_02_2 = (KC_0_02 * sqr_vely_plus_velY + KC_0_12 * vely_term_plus + KC_0_22) * (0.5);
    const Real KC_11_0 = KC_1_00 * oneminus_sqr_vely - KC_1_10 * (2.0) * velY - KC_1_20;
    const Real KC_10_0 = (KC_1_00 * sqr_vely_minus_velY + KC_1_10 * vely_term_minus + KC_1_20) * (0.5);
    const Real KC_12_0 = (KC_1_00 * sqr_vely_plus_velY + KC_1_10 * vely_term_plus + KC_1_20) * (0.5);
    const Real KC_11_1 = KC_1_01 * oneminus_sqr_vely - KC_1_11 * (2.0) * velY - KC_1_21;
    const Real KC_10_1 = (KC_1_01 * sqr_vely_minus_velY + KC_1_11 * vely_term_minus + KC_1_21) * (0.5);
    const Real KC_12_1 = (KC_1_01 * sqr_vely_plus_velY + KC_1_11 * vely_term_plus + KC_1_21) * (0.5);
    const Real KC_11_2 = KC_1_02 * oneminus_sqr_vely - KC_1_12 * (2.0) * velY - KC_1_22;
    const Real KC_10_2 = (KC_1_02 * sqr_vely_minus_velY + KC_1_12 * vely_term_minus + KC_1_22) * (0.5);
    const Real KC_12_2 = (KC_1_02 * sqr_vely_plus_velY + KC_1_12 * vely_term_plus + KC_1_22) * (0.5);
    const Real KC_21_0 = KC_2_00 * oneminus_sqr_vely - KC_2_10 * (2.0) * velY - KC_2_20;
    const Real KC_20_0 = (KC_2_00 * sqr_vely_minus_velY + KC_2_10 * vely_term_minus + KC_2_20) * (0.5);
    const Real KC_22_0 = (KC_2_00 * sqr_vely_plus_velY + KC_2_10 * vely_term_plus + KC_2_20) * (0.5);
    const Real KC_21_1 = KC_2_01 * oneminus_sqr_vely - KC_2_11 * (2.0) * velY - KC_2_21;
    const Real KC_20_1 = (KC_2_01 * sqr_vely_minus_velY + KC_2_11 * vely_term_minus + KC_2_21) * (0.5);
    const Real KC_22_1 = (KC_2_01 * sqr_vely_plus_velY + KC_2_11 * vely_term_plus + KC_2_21) * (0.5);
    const Real KC_21_2 = KC_2_02 * oneminus_sqr_vely - KC_2_12 * (2.0) * velY - KC_2_22;
    const Real KC_20_2 = (KC_2_02 * sqr_vely_minus_velY + KC_2_12 * vely_term_minus + KC_2_22) * (0.5);
    const Real KC_22_2 = (KC_2_02 * sqr_vely_plus_velY + KC_2_12 * vely_term_plus + KC_2_22) * (0.5);

    const Real oneminus_sqr_velz = (1.0) - velZZ;
    const Real sqr_velz_plus_velZ = velZZ + velZ;
    const Real sqr_velz_minus_velZ = velZZ - velZ;
    const Real velz_term_plus = (2.0) * velZ + (1.0);
    const Real velz_term_minus = (2.0) * velZ - (1.0);

    fold(i, j, k, 9) = KC_00_0 * oneminus_sqr_velz - KC_00_1 * (2.0) * velZ - KC_00_2;
    fold(i, j, k, 26) = (KC_00_0 * sqr_velz_minus_velZ + KC_00_1 * velz_term_minus + KC_00_2) * (0.5);
    fold(i, j, k, 22) = (KC_00_0 * sqr_velz_plus_velZ + KC_00_1 * velz_term_plus + KC_00_2) * (0.5);
    fold(i, j, k, 3) = KC_01_0 * oneminus_sqr_velz - KC_01_1 * (2.0) * velZ - KC_01_2;
    fold(i, j, k, 17) = (KC_01_0 * sqr_velz_minus_velZ + KC_01_1 * velz_term_minus + KC_01_2) * (0.5);
    fold(i, j, k, 13) = (KC_01_0 * sqr_velz_plus_velZ + KC_01_1 * velz_term_plus + KC_01_2) * (0.5);
    fold(i, j, k, 7) = KC_02_0 * oneminus_sqr_velz - KC_02_1 * (2.0) * velZ - KC_02_2;
    fold(i, j, k, 24) = (KC_02_0 * sqr_velz_minus_velZ + KC_02_1 * velz_term_minus + KC_02_2) * (0.5);
    fold(i, j, k, 20) = (KC_02_0 * sqr_velz_plus_velZ + KC_02_1 * velz_term_plus + KC_02_2) * (0.5);
    fold(i, j, k, 2) = KC_10_0 * oneminus_sqr_velz - KC_10_1 * (2.0) * velZ - KC_10_2;
    fold(i, j, k, 16) = (KC_10_0 * sqr_velz_minus_velZ + KC_10_1 * velz_term_minus + KC_10_2) * (0.5);
    fold(i, j, k, 12) = (KC_10_0 * sqr_velz_plus_velZ + KC_10_1 * velz_term_plus + KC_10_2) * (0.5);
    fold(i, j, k, 0) = KC_11_0 * oneminus_sqr_velz - KC_11_1 * (2.0) * velZ - KC_11_2;
    fold(i, j, k, 6) = (KC_11_0 * sqr_velz_minus_velZ + KC_11_1 * velz_term_minus + KC_11_2) * (0.5);
    fold(i, j, k, 5) = (KC_11_0 * sqr_velz_plus_velZ + KC_11_1 * velz_term_plus + KC_11_2) * (0.5);
    fold(i, j, k, 1) = KC_12_0 * oneminus_sqr_velz - KC_12_1 * (2.0) * velZ - KC_12_2;
    fold(i, j, k, 15) = (KC_12_0 * sqr_velz_minus_velZ + KC_12_1 * velz_term_minus + KC_12_2) * (0.5);
    fold(i, j, k, 11) = (KC_12_0 * sqr_velz_plus_velZ + KC_12_1 * velz_term_plus + KC_12_2) * (0.5);
    fold(i, j, k, 10) = KC_20_0 * oneminus_sqr_velz - KC_20_1 * (2.0) * velZ - KC_20_2;
    fold(i, j, k, 25) = (KC_20_0 * sqr_velz_minus_velZ + KC_20_1 * velz_term_minus + KC_20_2) * (0.5);
    fold(i, j, k, 21) = (KC_20_0 * sqr_velz_plus_velZ + KC_20_1 * velz_term_plus + KC_20_2) * (0.5);
    fold(i, j, k, 4) = KC_21_0 * oneminus_sqr_velz - KC_21_1 * (2.0) * velZ - KC_21_2;
    fold(i, j, k, 18) = (KC_21_0 * sqr_velz_minus_velZ + KC_21_1 * velz_term_minus + KC_21_2) * (0.5);
    fold(i, j, k, 14) = (KC_21_0 * sqr_velz_plus_velZ + KC_21_1 * velz_term_plus + KC_21_2) * (0.5);
    fold(i, j, k, 8) = KC_22_0 * oneminus_sqr_velz - KC_22_1 * (2.0) * velZ - KC_22_2;
    fold(i, j, k, 23) = (KC_22_0 * sqr_velz_minus_velZ + KC_22_1 * velz_term_minus + KC_22_2) * (0.5);
    fold(i, j, k, 19) = (KC_22_0 * sqr_velz_plus_velZ + KC_22_1 * velz_term_plus + KC_22_2) * (0.5);
}

// 分布函数空间转换的函数
AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void forwardInverseChimeraWithK(Real& mfa, Real& mfb, Real& mfc, Real vv, Real v2, Real Kinverse, Real K) {
    const Real m2 = mfa + mfc;
    const Real m1 = mfc - mfa;
    Real m0 = m2 + mfb;

    mfa = m0;
    m0 *= Kinverse;
    m0 += c1o1;
    mfb = (m1 * Kinverse - m0 * vv) * K;
    mfc = ((m2 - c2o1 * m1 * vv) * Kinverse + v2 * m0) * K;
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void backwardInverseChimeraWithK(Real& mfa, Real& mfb, Real& mfc, Real vv, Real v2, Real Kinverse, Real K) {
    const Real m0 = (((mfc - mfb) * c1o2 + mfb * vv) * Kinverse + (mfa * Kinverse + c1o1) * (v2 - vv) * c1o2) * K;
    const Real m1 = (((mfa - mfc) - c2o1 * mfb * vv) * Kinverse + (mfa * Kinverse + c1o1) * (-v2)) * K;

    mfc = (((mfc + mfb) * c1o2 + mfb * vv) * Kinverse + (mfa * Kinverse + c1o1) * (v2 + vv) * c1o2) * K;
    mfa = m0;
    mfb = m1;
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void forwardChimera(Real& mfa, Real& mfb, Real& mfc, Real vv, Real v2) {
    const Real m1 = (mfa + mfc) + mfb;
    const Real m2 = mfc - mfa;

    mfc = (mfc + mfa) + (v2 * m1 - c2o1 * vv * m2);
    mfb = m2 - vv * m1;
    mfa = m1;
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void backwardChimera(Real& mfa, Real& mfb, Real& mfc, Real vv, Real v2) {
    const Real ma = (mfc + mfa * (v2 - vv)) * c1o2 + mfb * (vv - c1o2);
    const Real mb = ((mfa - mfc) - mfa * v2) - c2o1 * mfb * vv;

    mfc = (mfc + mfa * (v2 + vv)) * c1o2 + mfb * (vv + c1o2);
    mfb = mb;
    mfa = ma;
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void forwardChimeraWithK(Real& mfa, Real& mfb, Real& mfc, Real vv, Real v2, Real K) {
    const Real m2 = mfa + mfc;
    const Real m1 = mfc - mfa;
    const Real m0 = m2 + mfb;
    mfa = m0;
    mfb = (m1 - K * vv) - m0 * vv;
    mfc = ((m2 - c2o1 * m1 * vv) + v2 * K) + v2 * m0;
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void backwardChimeraWithK(Real& mfa, Real& mfb, Real& mfc, Real vv, Real v2, Real K) {
    const Real m0 = (mfc - mfb) * c1o2 + mfb * (vv) + (mfa + K) * (v2 - vv) * c1o2;
    const Real m1 = (mfa - mfc) - c2o1 * mfb * vv + (mfa + K) * (-v2);
    mfc = (mfc + mfb) * c1o2 + mfb * (vv) + (mfa + K) * (v2 + vv) * c1o2;
    mfa = m0;
    mfb = m1;
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void collide_cumulant_opt(int i, int j, int k, amrex::Array4<amrex::Real> const& fold, amrex::Array4<amrex::Real> const& s,
                          amrex::Array4<amrex::Real> const& Ft, amrex::Real tau_lev, amrex::Real dt, amrex::IntVect const& hi) {
    // 常用参数定义在前面
    const Real quadricLimitP = 0.01;
    const Real quadricLimitM = 0.01;
    const Real quadricLimitD = 0.01;

    // 提取分布函数
    Real& f000 = fold(i, j, k, 0);
    Real& fP00 = fold(i, j, k, 1);
    Real& fM00 = fold(i, j, k, 2);
    Real& f0P0 = fold(i, j, k, 3);
    Real& f0M0 = fold(i, j, k, 4);
    Real& f00P = fold(i, j, k, 5);
    Real& f00M = fold(i, j, k, 6);
    Real& fPP0 = fold(i, j, k, 7);
    Real& fMM0 = fold(i, j, k, 8);
    Real& fPM0 = fold(i, j, k, 9);
    Real& fMP0 = fold(i, j, k, 10);
    Real& fP0P = fold(i, j, k, 11);
    Real& fM0M = fold(i, j, k, 12);
    Real& fP0M = fold(i, j, k, 13);
    Real& fM0P = fold(i, j, k, 14);
    Real& f0PP = fold(i, j, k, 15);
    Real& f0MM = fold(i, j, k, 16);
    Real& f0PM = fold(i, j, k, 17);
    Real& f0MP = fold(i, j, k, 18);
    Real& fPPP = fold(i, j, k, 19);
    Real& fMPP = fold(i, j, k, 20);
    Real& fPMP = fold(i, j, k, 21);
    Real& fMMP = fold(i, j, k, 22);
    Real& fPPM = fold(i, j, k, 23);
    Real& fMPM = fold(i, j, k, 24);
    Real& fPMM = fold(i, j, k, 25);
    Real& fMMM = fold(i, j, k, 26);

    // 别名分布函数
    Real& m111 = f000;
    Real& m211 = fP00;
    Real& m011 = fM00;
    Real& m121 = f0P0;
    Real& m101 = f0M0;
    Real& m112 = f00P;
    Real& m110 = f00M;
    Real& m221 = fPP0;
    Real& m001 = fMM0;
    Real& m201 = fPM0;
    Real& m021 = fMP0;
    Real& m212 = fP0P;
    Real& m010 = fM0M;
    Real& m210 = fP0M;
    Real& m012 = fM0P;
    Real& m122 = f0PP;
    Real& m100 = f0MM;
    Real& m120 = f0PM;
    Real& m102 = f0MP;
    Real& m222 = fPPP;
    Real& m022 = fMPP;
    Real& m202 = fPMP;
    Real& m002 = fMMP;
    Real& m220 = fPPM;
    Real& m020 = fMPM;
    Real& m200 = fPMM;
    Real& m000 = fMMM;

    // 计算宏观量
    Real ft, oneOverRho, Ftx, Fty, Ftz;
    Real drho = 0.0;
    Real vvx = 0.0;
    Real vvy = 0.0;
    Real vvz = 0.0;

    for (int q = 0; q < Q; q++) {
        ft = fold(i, j, k, q);
        drho += ft;
        vvx += ft * e[q][0];
        vvy += ft * e[q][1];
        vvz += ft * e[q][2];
    }
    oneOverRho = c1o1 / (drho + c1o1);
    vvx *= oneOverRho;
    vvy *= oneOverRho;
    vvz *= oneOverRho;

    Ftx = Ft(i, j, k, 0);
    Fty = Ft(i, j, k, 1);
    Ftz = Ft(i, j, k, 2);

    vvx += 0.5 * dt * Ftx * oneOverRho;
    vvy += 0.5 * dt * Fty * oneOverRho;
    vvz += 0.5 * dt * Ftz * oneOverRho;

    Real vx2 = vvx * vvx;
    Real vy2 = vvy * vvy;
    Real vz2 = vvz * vvz;

    forwardInverseChimeraWithK(fMMM, fMM0, fMMP, vvz, vz2, c36o1, c1o36);
    forwardInverseChimeraWithK(fM0M, fM00, fM0P, vvz, vz2, c9o1, c1o9);
    forwardInverseChimeraWithK(fMPM, fMP0, fMPP, vvz, vz2, c36o1, c1o36);
    forwardInverseChimeraWithK(f0MM, f0M0, f0MP, vvz, vz2, c9o1, c1o9);
    forwardInverseChimeraWithK(f00M, f000, f00P, vvz, vz2, c9o4, c4o9);
    forwardInverseChimeraWithK(f0PM, f0P0, f0PP, vvz, vz2, c9o1, c1o9);
    forwardInverseChimeraWithK(fPMM, fPM0, fPMP, vvz, vz2, c36o1, c1o36);
    forwardInverseChimeraWithK(fP0M, fP00, fP0P, vvz, vz2, c9o1, c1o9);
    forwardInverseChimeraWithK(fPPM, fPP0, fPPP, vvz, vz2, c36o1, c1o36);

    forwardInverseChimeraWithK(fMMM, fM0M, fMPM, vvy, vy2, c6o1, c1o6);
    forwardChimera(fMM0, fM00, fMP0, vvy, vy2);
    forwardInverseChimeraWithK(fMMP, fM0P, fMPP, vvy, vy2, c18o1, c1o18);
    forwardInverseChimeraWithK(f0MM, f00M, f0PM, vvy, vy2, c3o2, c2o3);
    forwardChimera(f0M0, f000, f0P0, vvy, vy2);
    forwardInverseChimeraWithK(f0MP, f00P, f0PP, vvy, vy2, c9o2, c2o9);
    forwardInverseChimeraWithK(fPMM, fP0M, fPPM, vvy, vy2, c6o1, c1o6);
    forwardChimera(fPM0, fP00, fPP0, vvy, vy2);
    forwardInverseChimeraWithK(fPMP, fP0P, fPPP, vvy, vy2, c18o1, c1o18);

    forwardInverseChimeraWithK(fMMM, f0MM, fPMM, vvx, vx2, c1o1, c1o1);
    forwardChimera(fM0M, f00M, fP0M, vvx, vx2);
    forwardInverseChimeraWithK(fMPM, f0PM, fPPM, vvx, vx2, c3o1, c1o3);
    forwardChimera(fMM0, f0M0, fPM0, vvx, vx2);
    forwardChimera(fM00, f000, fP00, vvx, vx2);
    forwardChimera(fMP0, f0P0, fPP0, vvx, vx2);
    forwardInverseChimeraWithK(fMMP, f0MP, fPMP, vvx, vx2, c3o1, c1o3);
    forwardChimera(fM0P, f00P, fP0P, vvx, vx2);
    forwardInverseChimeraWithK(fMPP, f0PP, fPPP, vvx, vx2, c3o1, c1o9);

    // LES在这里加
    Real omega = 1.0 / tau_lev;
    Real OxxPyyPzz = c1o1;
    Real OxyyPxzz = c8o1 * (-c2o1 + omega) * (c1o1 + c2o1 * omega) / (-c8o1 - c14o1 * omega + c7o1 * omega * omega);
    Real OxyyMxzz = c8o1 * (-c2o1 + omega) * (-c7o1 + c4o1 * omega) / (c56o1 - c50o1 * omega + c9o1 * omega * omega);
    Real Oxyz = c24o1 * (-c2o1 + omega) * (-c2o1 - c7o1 * omega + c3o1 * omega * omega) / (c48o1 + c152o1 * omega - c130o1 * omega * omega + c29o1 * omega * omega * omega);
    Real O4 = c1o1;
    Real O5 = c1o1;
    Real O6 = c1o1;
    Real factorA = (c4o1 + c2o1 * omega - c3o1 * omega * omega) / (c2o1 - c7o1 * omega + c5o1 * omega * omega);
    Real factorB = (c4o1 + c28o1 * omega - c14o1 * omega * omega) / (c6o1 - c21o1 * omega + c15o1 * omega * omega);

    Real cm211 = m211 - ((m200 + c1o3) * m011 + c2o1 * m110 * m101) * oneOverRho;
    Real cm121 = m121 - ((m020 + c1o3) * m101 + c2o1 * m110 * m011) * oneOverRho;
    Real cm112 = m112 - ((m002 + c1o3) * m110 + c2o1 * m101 * m011) * oneOverRho;

    Real cm220 = m220 - (((m200 * m020 + c2o1 * m110 * m110) + c1o3 * (m200 + m020)) * oneOverRho - c1o9 * (drho * oneOverRho));
    Real cm202 = m202 - (((m200 * m002 + c2o1 * m101 * m101) + c1o3 * (m200 + m002)) * oneOverRho - c1o9 * (drho * oneOverRho));
    Real cm022 = m022 - (((m002 * m020 + c2o1 * m011 * m011) + c1o3 * (m002 + m020)) * oneOverRho - c1o9 * (drho * oneOverRho));
    ////////////////////////////////////////////////////////////
    // 5.
    Real cm122 =
        m122 - ((m002 * m120 + m020 * m102 + c4o1 * m011 * m111 + c2o1 * (m101 * m021 + m110 * m012)) +
                c1o3 * (m120 + m102)) *
                   oneOverRho;
    Real cm212 =
        m212 - ((m002 * m210 + m200 * m012 + c4o1 * m101 * m111 + c2o1 * (m011 * m201 + m110 * m102)) +
                c1o3 * (m210 + m012)) *
                   oneOverRho;
    Real cm221 =
        m221 - ((m200 * m021 + m020 * m201 + c4o1 * m110 * m111 + c2o1 * (m101 * m120 + m011 * m210)) +
                c1o3 * (m021 + m201)) *
                   oneOverRho;
    ////////////////////////////////////////////////////////////
    // 6.
    Real cm222 = m222 + ((-c4o1 * m111 * m111 - (m200 * m022 + m020 * m202 + m002 * m220) -
                          c4o1 * (m011 * m211 + m101 * m121 + m110 * m112) -
                          c2o1 * (m120 * m102 + m210 * m012 + m201 * m021)) *
                             oneOverRho +
                         (c4o1 * (m101 * m101 * m020 + m011 * m011 * m200 + m110 * m110 * m002) +
                          c2o1 * (m200 * m020 * m002) + c16o1 * m110 * m101 * m011) *
                             oneOverRho * oneOverRho -
                         c1o3 * (m022 + m202 + m220) * oneOverRho - c1o9 * (m200 + m020 + m002) * oneOverRho +
                         (c2o1 * (m101 * m101 + m011 * m011 + m110 * m110) +
                          (m002 * m020 + m002 * m200 + m020 * m200) + c1o3 * (m002 + m020 + m200)) *
                             oneOverRho * oneOverRho * c2o3 +
                         c1o27 * ((drho * drho - drho) * oneOverRho * oneOverRho));

    ////////////////////////////////////////////////////////////////////////////////////
    //! - Compute linear combinations of second and third order cumulants
    // 2.
    Real mxxPyyPzz = m200 + m020 + m002;
    Real mxxMyy = m200 - m020;
    Real mxxMzz = m200 - m002;
    ////////////////////////////////////////////////////////////
    // 3.
    Real mxxyPyzz = m210 + m012;
    Real mxxyMyzz = m210 - m012;

    Real mxxzPyyz = m201 + m021;
    Real mxxzMyyz = m201 - m021;

    Real mxyyPxzz = m120 + m102;
    Real mxyyMxzz = m120 - m102;

    ////////////////////////////////////////////////////////////////////////////////////
    // incl. correction
    Real Dxy = -c3o1 * omega * m110;
    Real Dxz = -c3o1 * omega * m101;
    Real Dyz = -c3o1 * omega * m011;
    Real dxux = c1o2 * (-omega) * (mxxMyy + mxxMzz) + c1o2 * OxxPyyPzz * (m000 - mxxPyyPzz);
    Real dyuy = dxux + omega * c3o2 * mxxMyy;
    Real dzuz = dxux + omega * c3o2 * mxxMzz;

    ////////////////////////////////////////////////////////////////////////////////////
    // 可以调用一些LES模型
    ////////////////////////////////////////////////////////////
    mxxPyyPzz += OxxPyyPzz * (m000 - mxxPyyPzz) - c3o1 * (c1o1 - c1o2 * OxxPyyPzz) * (vx2 * dxux + vy2 * dyuy + vz2 * dzuz);
    mxxMyy += omega * (-mxxMyy) - c3o1 * (c1o1 + c1o2 * (-omega)) * (vx2 * dxux - vy2 * dyuy);
    mxxMzz += omega * (-mxxMzz) - c3o1 * (c1o1 + c1o2 * (-omega)) * (vx2 * dxux - vz2 * dzuz);
    //////////////////////////////////////////////////////////////////////////
    m011 += omega * (-m011);
    m101 += omega * (-m101);
    m110 += omega * (-m110);
    ////////////////////////////////////////////////////////////////////////////////////
    // relax
    Real wadjust = Oxyz + (c1o1 - Oxyz) * KERNEL_ABS(m111) / (KERNEL_ABS(m111) + quadricLimitD);
    m111 += wadjust * (-m111);
    wadjust = OxyyPxzz + (c1o1 - OxyyPxzz) * KERNEL_ABS(mxxyPyzz) / (KERNEL_ABS(mxxyPyzz) + quadricLimitP);
    mxxyPyzz += wadjust * (-mxxyPyzz);
    wadjust = OxyyMxzz + (c1o1 - OxyyMxzz) * KERNEL_ABS(mxxyMyzz) / (KERNEL_ABS(mxxyMyzz) + quadricLimitM);
    mxxyMyzz += wadjust * (-mxxyMyzz);
    wadjust = OxyyPxzz + (c1o1 - OxyyPxzz) * KERNEL_ABS(mxxzPyyz) / (KERNEL_ABS(mxxzPyyz) + quadricLimitP);
    mxxzPyyz += wadjust * (-mxxzPyyz);
    wadjust = OxyyMxzz + (c1o1 - OxyyMxzz) * KERNEL_ABS(mxxzMyyz) / (KERNEL_ABS(mxxzMyyz) + quadricLimitM);
    mxxzMyyz += wadjust * (-mxxzMyyz);
    wadjust = OxyyPxzz + (c1o1 - OxyyPxzz) * KERNEL_ABS(mxyyPxzz) / (KERNEL_ABS(mxyyPxzz) + quadricLimitP);
    mxyyPxzz += wadjust * (-mxyyPxzz);
    wadjust = OxyyMxzz + (c1o1 - OxyyMxzz) * KERNEL_ABS(mxyyMxzz) / (KERNEL_ABS(mxyyMxzz) + quadricLimitM);
    mxyyMxzz += wadjust * (-mxyyMxzz);

    ////////////////////////////////////////////////////////////////////////////////////
    m200 = c1o3 * (mxxMyy + mxxMzz + mxxPyyPzz);
    m020 = c1o3 * (-c2o1 * mxxMyy + mxxMzz + mxxPyyPzz);
    m002 = c1o3 * (mxxMyy - c2o1 * mxxMzz + mxxPyyPzz);

    m210 = (mxxyMyzz + mxxyPyzz) * c1o2;
    m012 = (-mxxyMyzz + mxxyPyzz) * c1o2;
    m201 = (mxxzMyyz + mxxzPyyz) * c1o2;
    m021 = (-mxxzMyyz + mxxzPyyz) * c1o2;
    m120 = (mxyyMxzz + mxyyPxzz) * c1o2;
    m102 = (-mxyyMxzz + mxyyPxzz) * c1o2;
    //////////////////////////////////////////////////////////////////////////
    cm022 = -O4 * (c1o1 / omega - c1o2) * (dyuy + dzuz) * c2o3 * factorA + (c1o1 - O4) * (cm022);
    cm202 = -O4 * (c1o1 / omega - c1o2) * (dxux + dzuz) * c2o3 * factorA + (c1o1 - O4) * (cm202);
    cm220 = -O4 * (c1o1 / omega - c1o2) * (dyuy + dxux) * c2o3 * factorA + (c1o1 - O4) * (cm220);
    cm112 = -O4 * (c1o1 / omega - c1o2) * Dxy * c1o3 * factorB + (c1o1 - O4) * (cm112);
    cm121 = -O4 * (c1o1 / omega - c1o2) * Dxz * c1o3 * factorB + (c1o1 - O4) * (cm121);
    cm211 = -O4 * (c1o1 / omega - c1o2) * Dyz * c1o3 * factorB + (c1o1 - O4) * (cm211);

    //////////////////////////////////////////////////////////////////////////
    // 5.
    cm122 += O5 * (-cm122);
    cm212 += O5 * (-cm212);
    cm221 += O5 * (-cm221);

    //////////////////////////////////////////////////////////////////////////
    // 6.
    cm222 += O6 * (-cm222);
    //////////////////////////////////////////////////////////////////////////
    // 4.
    m211 = cm211 + c1o3 * ((c3o1 * m200 + c1o1) * m011 + c6o1 * m110 * m101) * oneOverRho;
    m121 = cm121 + c1o3 * ((c3o1 * m020 + c1o1) * m101 + c6o1 * m110 * m011) * oneOverRho;
    m112 = cm112 + c1o3 * ((c3o1 * m002 + c1o1) * m110 + c6o1 * m101 * m011) * oneOverRho;

    m220 =
        cm220 + (((m200 * m020 + c2o1 * m110 * m110) * c9o1 + c3o1 * (m200 + m020)) * oneOverRho - (drho * oneOverRho)) * c1o9;
    m202 =
        cm202 + (((m200 * m002 + c2o1 * m101 * m101) * c9o1 + c3o1 * (m200 + m002)) * oneOverRho - (drho * oneOverRho)) * c1o9;
    m022 =
        cm022 + (((m002 * m020 + c2o1 * m011 * m011) * c9o1 + c3o1 * (m002 + m020)) * oneOverRho - (drho * oneOverRho)) * c1o9;

    //////////////////////////////////////////////////////////////////////////
    // 5.
    m122 = cm122 + c1o3 *
                       (c3o1 * (m002 * m120 + m020 * m102 + c4o1 * m011 * m111 + c2o1 * (m101 * m021 + m110 * m012)) +
                        (m120 + m102)) *
                       oneOverRho;
    m212 = cm212 + c1o3 *
                       (c3o1 * (m002 * m210 + m200 * m012 + c4o1 * m101 * m111 + c2o1 * (m011 * m201 + m110 * m102)) +
                        (m210 + m012)) *
                       oneOverRho;
    m221 = cm221 + c1o3 *
                       (c3o1 * (m200 * m021 + m020 * m201 + c4o1 * m110 * m111 + c2o1 * (m101 * m120 + m011 * m210)) +
                        (m021 + m201)) *
                       oneOverRho;

    //////////////////////////////////////////////////////////////////////////
    // 6.
    m222 = cm222 - ((-c4o1 * m111 * m111 - (m200 * m022 + m020 * m202 + m002 * m220) -
                     c4o1 * (m011 * m211 + m101 * m121 + m110 * m112) -
                     c2o1 * (m120 * m102 + m210 * m012 + m201 * m021)) *
                        oneOverRho +
                    (c4o1 * (m101 * m101 * m020 + m011 * m011 * m200 + m110 * m110 * m002) +
                     c2o1 * (m200 * m020 * m002) + c16o1 * m110 * m101 * m011) *
                        oneOverRho * oneOverRho -
                    c1o3 * (m022 + m202 + m220) * oneOverRho - c1o9 * (m200 + m020 + m002) * oneOverRho +
                    (c2o1 * (m101 * m101 + m011 * m011 + m110 * m110) +
                     (m002 * m020 + m002 * m200 + m020 * m200) + c1o3 * (m002 + m020 + m200)) *
                        oneOverRho * oneOverRho * c2o3 +
                    c1o27 * ((drho * drho - drho) * oneOverRho * oneOverRho));

    ////////////////////////////////////////////////////////////////////////////////////
    m100 = -m100;
    m010 = -m010;
    m001 = -m001;

    ////////////////////////////////////////////////////////////////////////////////////
    // X - Dir
    backwardInverseChimeraWithK(m000, m100, m200, vvx, vx2, c1o1, c1o1);
    backwardChimera(m010, m110, m210, vvx, vx2);
    backwardInverseChimeraWithK(m020, m120, m220, vvx, vx2, c3o1, c1o3);
    backwardChimera(m001, m101, m201, vvx, vx2);
    backwardChimera(m011, m111, m211, vvx, vx2);
    backwardChimera(m021, m121, m221, vvx, vx2);
    backwardInverseChimeraWithK(m002, m102, m202, vvx, vx2, c3o1, c1o3);
    backwardChimera(m012, m112, m212, vvx, vx2);
    backwardInverseChimeraWithK(m022, m122, m222, vvx, vx2, c9o1, c1o9);

    ////////////////////////////////////////////////////////////////////////////////////
    // Y - Dir
    backwardInverseChimeraWithK(m000, m010, m020, vvy, vy2, c6o1, c1o6);
    backwardChimera(m001, m011, m021, vvy, vy2);
    backwardInverseChimeraWithK(m002, m012, m022, vvy, vy2, c18o1, c1o18);
    backwardInverseChimeraWithK(m100, m110, m120, vvy, vy2, c3o2, c2o3);
    backwardChimera(m101, m111, m121, vvy, vy2);
    backwardInverseChimeraWithK(m102, m112, m122, vvy, vy2, c9o2, c2o9);
    backwardInverseChimeraWithK(m200, m210, m220, vvy, vy2, c6o1, c1o6);
    backwardChimera(m201, m211, m221, vvy, vy2);
    backwardInverseChimeraWithK(m202, m212, m222, vvy, vy2, c18o1, c1o18);

    ////////////////////////////////////////////////////////////////////////////////////
    // Z - Dir
    backwardInverseChimeraWithK(m000, m001, m002, vvz, vz2, c36o1, c1o36);
    backwardInverseChimeraWithK(m010, m011, m012, vvz, vz2, c9o1, c1o9);
    backwardInverseChimeraWithK(m020, m021, m022, vvz, vz2, c36o1, c1o36);
    backwardInverseChimeraWithK(m100, m101, m102, vvz, vz2, c9o1, c1o9);
    backwardInverseChimeraWithK(m110, m111, m112, vvz, vz2, c9o4, c4o9);
    backwardInverseChimeraWithK(m120, m121, m122, vvz, vz2, c9o1, c1o9);
    backwardInverseChimeraWithK(m200, m201, m202, vvz, vz2, c36o1, c1o36);
    backwardInverseChimeraWithK(m210, m211, m212, vvz, vz2, c9o1, c1o9);
    backwardInverseChimeraWithK(m220, m221, m222, vvz, vz2, c36o1, c1o36);

    // 分布函数回代
    fold(i, j, k, 1) = fP00;
    fold(i, j, k, 2) = fM00;
    fold(i, j, k, 3) = f0P0;
    fold(i, j, k, 4) = f0M0;
    fold(i, j, k, 5) = f00P;
    fold(i, j, k, 6) = f00M;
    fold(i, j, k, 7) = fPP0;
    fold(i, j, k, 8) = fMM0;
    fold(i, j, k, 9) = fPM0;
    fold(i, j, k, 10) = fMP0;
    fold(i, j, k, 11) = fP0P;
    fold(i, j, k, 12) = fM0M;
    fold(i, j, k, 13) = fP0M;
    fold(i, j, k, 14) = fM0P;
    fold(i, j, k, 15) = f0PP;
    fold(i, j, k, 16) = f0MM;
    fold(i, j, k, 17) = f0PM;
    fold(i, j, k, 18) = f0MP;
    fold(i, j, k, 0) = f000;
    fold(i, j, k, 19) = fPPP;
    fold(i, j, k, 21) = fPMP;
    fold(i, j, k, 23) = fPPM;
    fold(i, j, k, 25) = fPMM;
    fold(i, j, k, 20) = fMPP;
    fold(i, j, k, 22) = fMMP;
    fold(i, j, k, 24) = fMPM;
    fold(i, j, k, 26) = fMMM;
}

// 七月份的那一版
AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void collide_cumulant_opt2(int i, int j, int k, amrex::Array4<amrex::Real> const& fold, amrex::Array4<amrex::Real> const& s,
                           amrex::Array4<amrex::Real> const& Ft, amrex::Real tau_lev, amrex::Real dt, amrex::IntVect const& hi) {
    // 需要用到的一些常量参数
    const Real quadricLimitP = 0.01;
    const Real quadricLimitM = 0.01;
    const Real quadricLimitD = 0.01;

    // 取出分布函数
    Real& f000 = fold(i, j, k, 0);
    Real& fP00 = fold(i, j, k, 1);
    Real& fM00 = fold(i, j, k, 2);
    Real& f0P0 = fold(i, j, k, 3);
    Real& f0M0 = fold(i, j, k, 4);
    Real& f00P = fold(i, j, k, 5);
    Real& f00M = fold(i, j, k, 6);
    Real& fPP0 = fold(i, j, k, 7);
    Real& fMM0 = fold(i, j, k, 8);
    Real& fPM0 = fold(i, j, k, 9);
    Real& fMP0 = fold(i, j, k, 10);
    Real& fP0P = fold(i, j, k, 11);
    Real& fM0M = fold(i, j, k, 12);
    Real& fP0M = fold(i, j, k, 13);
    Real& fM0P = fold(i, j, k, 14);
    Real& f0PP = fold(i, j, k, 15);
    Real& f0MM = fold(i, j, k, 16);
    Real& f0PM = fold(i, j, k, 17);
    Real& f0MP = fold(i, j, k, 18);
    Real& fPPP = fold(i, j, k, 19);
    Real& fMPP = fold(i, j, k, 20);
    Real& fPMP = fold(i, j, k, 21);
    Real& fMMP = fold(i, j, k, 22);
    Real& fPPM = fold(i, j, k, 23);
    Real& fMPM = fold(i, j, k, 24);
    Real& fPMM = fold(i, j, k, 25);
    Real& fMMM = fold(i, j, k, 26);

    Real& m111 = f000;
    Real& m211 = fP00;
    Real& m011 = fM00;
    Real& m121 = f0P0;
    Real& m101 = f0M0;
    Real& m112 = f00P;
    Real& m110 = f00M;
    Real& m221 = fPP0;
    Real& m001 = fMM0;
    Real& m201 = fPM0;
    Real& m021 = fMP0;
    Real& m212 = fP0P;
    Real& m010 = fM0M;
    Real& m210 = fP0M;
    Real& m012 = fM0P;
    Real& m122 = f0PP;
    Real& m100 = f0MM;
    Real& m120 = f0PM;
    Real& m102 = f0MP;
    Real& m222 = fPPP;
    Real& m022 = fMPP;
    Real& m202 = fPMP;
    Real& m002 = fMMP;
    Real& m220 = fPPM;
    Real& m020 = fMPM;
    Real& m200 = fPMM;
    Real& m000 = fMMM;

    // 计算宏观量
    Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;
    Real Ftx = 0.0, Fty = 0.0, Ftz = 0.0;

    for (int q = 0; q < Q; q++) {
        ft = fold(i, j, k, q);
        rhot += ft;
        uxt += ft * e[q][0];
        uyt += ft * e[q][1];
        uzt += ft * e[q][2];
    }

    Real drho = rhot;
    Real oneOverRho = 1.0 / (drho + 1.0);

    Ftx = Ft(i, j, k, 0);
    Fty = Ft(i, j, k, 1);
    Ftz = Ft(i, j, k, 2);

    uxt *= oneOverRho;
    uyt *= oneOverRho;
    uzt *= oneOverRho;

    // 这里不知道要不要修正一下
    uxt += 0.5 * dt * Ftx * oneOverRho;
    uyt += 0.5 * dt * Fty * oneOverRho;
    uzt += 0.5 * dt * Ftz * oneOverRho;

    const Real vvx = uxt;
    const Real vvy = uyt;
    const Real vvz = uzt;

    const Real vx2 = uxt * uxt;
    const Real vy2 = uyt * uyt;
    const Real vz2 = uzt * uzt;

    ////////////////////////////////////////////////////////////////////////////////////
    //! - Chimera transform from well conditioned distributions to central moments as defined in Appendix J in \ref
    //! <a href="https://doi.org/10.1016/j.camwa.2015.05.001"><b>[ M. Geier et al. (2015),
    //! DOI:10.1016/j.camwa.2015.05.001 ]</b></a> see also Eq. (6)-(14) in \ref <a
    //! href="https://doi.org/10.1016/j.jcp.2017.05.040"><b>[ M. Geier et al. (2017), DOI:10.1016/j.jcp.2017.05.040
    //! ]</b></a>
    //!
    ////////////////////////////////////////////////////////////////////////////////////
    // Z - Dir
    forwardInverseChimeraWithK(fMMM, fMM0, fMMP, vvz, vz2, c36o1, c1o36);
    forwardInverseChimeraWithK(fM0M, fM00, fM0P, vvz, vz2, c9o1, c1o9);
    forwardInverseChimeraWithK(fMPM, fMP0, fMPP, vvz, vz2, c36o1, c1o36);
    forwardInverseChimeraWithK(f0MM, f0M0, f0MP, vvz, vz2, c9o1, c1o9);
    forwardInverseChimeraWithK(f00M, f000, f00P, vvz, vz2, c9o4, c4o9);
    forwardInverseChimeraWithK(f0PM, f0P0, f0PP, vvz, vz2, c9o1, c1o9);
    forwardInverseChimeraWithK(fPMM, fPM0, fPMP, vvz, vz2, c36o1, c1o36);
    forwardInverseChimeraWithK(fP0M, fP00, fP0P, vvz, vz2, c9o1, c1o9);
    forwardInverseChimeraWithK(fPPM, fPP0, fPPP, vvz, vz2, c36o1, c1o36);

    ////////////////////////////////////////////////////////////////////////////////////
    // Y - Dir
    forwardInverseChimeraWithK(fMMM, fM0M, fMPM, vvy, vy2, c6o1, c1o6);
    forwardChimera(fMM0, fM00, fMP0, vvy, vy2);
    forwardInverseChimeraWithK(fMMP, fM0P, fMPP, vvy, vy2, c18o1, c1o18);
    forwardInverseChimeraWithK(f0MM, f00M, f0PM, vvy, vy2, c3o2, c2o3);
    forwardChimera(f0M0, f000, f0P0, vvy, vy2);
    forwardInverseChimeraWithK(f0MP, f00P, f0PP, vvy, vy2, c9o2, c2o9);
    forwardInverseChimeraWithK(fPMM, fP0M, fPPM, vvy, vy2, c6o1, c1o6);
    forwardChimera(fPM0, fP00, fPP0, vvy, vy2);
    forwardInverseChimeraWithK(fPMP, fP0P, fPPP, vvy, vy2, c18o1, c1o18);

    ////////////////////////////////////////////////////////////////////////////////////
    // X - Dir
    forwardInverseChimeraWithK(fMMM, f0MM, fPMM, vvx, vx2, c1o1, c1o1);
    forwardChimera(fM0M, f00M, fP0M, vvx, vx2);
    forwardInverseChimeraWithK(fMPM, f0PM, fPPM, vvx, vx2, c3o1, c1o3);
    forwardChimera(fMM0, f0M0, fPM0, vvx, vx2);
    forwardChimera(fM00, f000, fP00, vvx, vx2);
    forwardChimera(fMP0, f0P0, fPP0, vvx, vx2);
    forwardInverseChimeraWithK(fMMP, f0MP, fPMP, vvx, vx2, c3o1, c1o3);
    forwardChimera(fM0P, f00P, fP0P, vvx, vx2);
    forwardInverseChimeraWithK(fMPP, f0PP, fPPP, vvx, vx2, c3o1, c1o9);

    ////////////////////////////////////////////////////////////////////////////////////
    //! - Setting relaxation rates for non-hydrodynamic cumulants (default values). Variable names and equations
    //! according to <a href="https://doi.org/10.1016/j.jcp.2017.05.040"><b>[ M. Geier et al. (2017),
    //! DOI:10.1016/j.jcp.2017.05.040 ]</b></a>
    //!  => [NAME IN PAPER]=[NAME IN CODE]=[DEFAULT VALUE].
    //!  - Trace of second order cumulants \f$ C{200}+C{020}+C{002} \f$ used to adjust bulk
    //!  viscosity:\f$\omega2=OxxPyyPzz=1.0 \f$.
    //!  - Third order cumulants \f$ C{120}+C{102}, C{210}+C{012}, C{201}+C{021} \f$: \f$ \omega3=OxyyPxzz
    //!  \f$ set according to Eq. (111) with simplifications assuming \f$ \omega2=1.0\f$.
    //!  - Third order cumulants \f$ C{120}-C{102}, C{210}-C{012}, C{201}-C{021} \f$: \f$ \omega4 = OxyyMxzz
    //!  \f$ set according to Eq. (112) with simplifications assuming \f$ \omega2 = 1.0\f$.
    //!  - Third order cumulants \f$ C{111} \f$: \f$ \omega5 = Oxyz \f$ set according to Eq. (113) with
    //!  simplifications assuming \f$ \omega2 = 1.0\f$  (modify for different bulk viscosity).
    //!  - Fourth order cumulants \f$ C{220}, C{202}, C{022}, C{211}, C{121}, C{112} \f$: for simplification
    //!  all set to the same default value \f$ \omega6=\omega7=\omega8=O4=1.0 \f$.
    //!  - Fifth order cumulants \f$ C{221}, C{212}, C{122}\f$: \f$\omega9=O5=1.0\f$.
    //!  - Sixth order cumulant \f$ C{222}\f$: \f$\omega{10}=O6=1.0\f$.
    //!
    ////////////////////////////////////////////////////////////////////////////////////
    //! - Calculate modified omega with turbulent viscosity
    //!
    const Real omega = 1.0 / tau_lev;
    ////////////////////////////////////////////////////////////
    // 2.
    Real OxxPyyPzz = c1o1;
    ////////////////////////////////////////////////////////////
    // 3.
    Real OxyyPxzz = c8o1 * (-c2o1 + omega) * (c1o1 + c2o1 * omega) / (-c8o1 - c14o1 * omega + c7o1 * omega * omega);
    Real OxyyMxzz =
        c8o1 * (-c2o1 + omega) * (-c7o1 + c4o1 * omega) / (c56o1 - c50o1 * omega + c9o1 * omega * omega);
    Real Oxyz = c24o1 * (-c2o1 + omega) * (-c2o1 - c7o1 * omega + c3o1 * omega * omega) /
                (c48o1 + c152o1 * omega - c130o1 * omega * omega + c29o1 * omega * omega * omega);
    ////////////////////////////////////////////////////////////
    // 4.
    Real O4 = c1o1;
    ////////////////////////////////////////////////////////////
    // 5.
    Real O5 = c1o1;
    ////////////////////////////////////////////////////////////
    // 6.
    Real O6 = c1o1;

    ////////////////////////////////////////////////////////////////////////////////////
    //! - A and d00M: parameters for fourth order convergence of the diffusion term according to Eq. (115) and (116)
    //! <a href="https://doi.org/10.1016/j.jcp.2017.05.040"><b>[ M. Geier et al. (2017),
    //! DOI:10.1016/j.jcp.2017.05.040 ]</b></a> with simplifications assuming \f$ \omega_2 = 1.0 \f$ (modify for
    //! different bulk viscosity).
    //!
    Real factorA = (c4o1 + c2o1 * omega - c3o1 * omega * omega) / (c2o1 - c7o1 * omega + c5o1 * omega * omega);
    Real factorB = (c4o1 + c28o1 * omega - c14o1 * omega * omega) / (c6o1 - c21o1 * omega + c15o1 * omega * omega);

    ////////////////////////////////////////////////////////////////////////////////////
    //! - Compute cumulants from central moments according to Eq. (20)-(23) in
    //! <a href="https://doi.org/10.1016/j.jcp.2017.05.040"><b>[ M. Geier et al. (2017),
    //! DOI:10.1016/j.jcp.2017.05.040 ]</b></a>
    //!
    ////////////////////////////////////////////////////////////
    // 4.
    Real cm211 = m211 - ((m200 + c1o3) * m011 + c2o1 * m110 * m101) * oneOverRho;
    Real cm121 = m121 - ((m020 + c1o3) * m101 + c2o1 * m110 * m011) * oneOverRho;
    Real cm112 = m112 - ((m002 + c1o3) * m110 + c2o1 * m101 * m011) * oneOverRho;

    Real cm220 = m220 - (((m200 * m020 + c2o1 * m110 * m110) + c1o3 * (m200 + m020)) * oneOverRho - c1o9 * (drho * oneOverRho));
    Real cm202 = m202 - (((m200 * m002 + c2o1 * m101 * m101) + c1o3 * (m200 + m002)) * oneOverRho - c1o9 * (drho * oneOverRho));
    Real cm022 = m022 - (((m002 * m020 + c2o1 * m011 * m011) + c1o3 * (m002 + m020)) * oneOverRho - c1o9 * (drho * oneOverRho));
    ////////////////////////////////////////////////////////////
    // 5.
    Real cm122 =
        m122 - ((m002 * m120 + m020 * m102 + c4o1 * m011 * m111 + c2o1 * (m101 * m021 + m110 * m012)) +
                c1o3 * (m120 + m102)) *
                   oneOverRho;
    Real cm212 =
        m212 - ((m002 * m210 + m200 * m012 + c4o1 * m101 * m111 + c2o1 * (m011 * m201 + m110 * m102)) +
                c1o3 * (m210 + m012)) *
                   oneOverRho;
    Real cm221 =
        m221 - ((m200 * m021 + m020 * m201 + c4o1 * m110 * m111 + c2o1 * (m101 * m120 + m011 * m210)) +
                c1o3 * (m021 + m201)) *
                   oneOverRho;
    ////////////////////////////////////////////////////////////
    // 6.
    Real cm222 = m222 + ((-c4o1 * m111 * m111 - (m200 * m022 + m020 * m202 + m002 * m220) -
                          c4o1 * (m011 * m211 + m101 * m121 + m110 * m112) -
                          c2o1 * (m120 * m102 + m210 * m012 + m201 * m021)) *
                             oneOverRho +
                         (c4o1 * (m101 * m101 * m020 + m011 * m011 * m200 + m110 * m110 * m002) +
                          c2o1 * (m200 * m020 * m002) + c16o1 * m110 * m101 * m011) *
                             oneOverRho * oneOverRho -
                         c1o3 * (m022 + m202 + m220) * oneOverRho - c1o9 * (m200 + m020 + m002) * oneOverRho +
                         (c2o1 * (m101 * m101 + m011 * m011 + m110 * m110) +
                          (m002 * m020 + m002 * m200 + m020 * m200) + c1o3 * (m002 + m020 + m200)) *
                             oneOverRho * oneOverRho * c2o3 +
                         c1o27 * ((drho * drho - drho) * oneOverRho * oneOverRho));

    ////////////////////////////////////////////////////////////////////////////////////
    //! - Compute linear combinations of second and third order cumulants
    //!
    ////////////////////////////////////////////////////////////
    // 2.
    Real mxxPyyPzz = m200 + m020 + m002;
    Real mxxMyy = m200 - m020;
    Real mxxMzz = m200 - m002;
    ////////////////////////////////////////////////////////////
    // 3.
    Real mxxyPyzz = m210 + m012;
    Real mxxyMyzz = m210 - m012;

    Real mxxzPyyz = m201 + m021;
    Real mxxzMyyz = m201 - m021;

    Real mxyyPxzz = m120 + m102;
    Real mxyyMxzz = m120 - m102;

    ////////////////////////////////////////////////////////////////////////////////////
    // incl. correction
    ////////////////////////////////////////////////////////////
    //! - Compute velocity  gradients from second order cumulants according to Eq. (27)-(32)
    //! <a href="https://doi.org/10.1016/j.jcp.2017.05.040"><b>[ M. Geier et al. (2017),
    //! DOI:10.1016/j.jcp.2017.05.040 ]</b></a> Further explanations of the correction in viscosity in Appendix H of
    //! <a href="https://doi.org/10.1016/j.camwa.2015.05.001"><b>[ M. Geier et al. (2015),
    //! DOI:10.1016/j.camwa.2015.05.001 ]</b></a> Note that the division by rho is omitted here as we need rho times
    //! the gradients later.RR
    //!
    Real Dxy = -c3o1 * omega * m110;
    Real Dxz = -c3o1 * omega * m101;
    Real Dyz = -c3o1 * omega * m011;
    Real dxux = c1o2 * (-omega) * (mxxMyy + mxxMzz) + c1o2 * OxxPyyPzz * (m000 - mxxPyyPzz);
    Real dyuy = dxux + omega * c3o2 * mxxMyy;
    Real dzuz = dxux + omega * c3o2 * mxxMzz;

    ////////////////////////////////////////////////////////////////////////////////////
    // 可以调用一些LES模型
    ////////////////////////////////////////////////////////////

    //! - Relaxation of second order cumulants with correction terms according to Eq. (33)-(35) in
    //! <a href="https://doi.org/10.1016/j.jcp.2017.05.040"><b>[ M. Geier et al. (2017),
    //! DOI:10.1016/j.jcp.2017.05.040 ]</b></a>
    //!
    mxxPyyPzz += OxxPyyPzz * (m000 - mxxPyyPzz) - c3o1 * (c1o1 - c1o2 * OxxPyyPzz) * (vx2 * dxux + vy2 * dyuy + vz2 * dzuz);
    mxxMyy += omega * (-mxxMyy) - c3o1 * (c1o1 + c1o2 * (-omega)) * (vx2 * dxux - vy2 * dyuy);
    mxxMzz += omega * (-mxxMzz) - c3o1 * (c1o1 + c1o2 * (-omega)) * (vx2 * dxux - vz2 * dzuz);

    ////////////////////////////////////////////////////////////////////////////////////
    ////no correction
    // mxxPyyPzz += OxxPyyPzz*(mfaaa - mxxPyyPzz);
    // mxxMyy += -(-omega) * (-mxxMyy);
    // mxxMzz += -(-omega) * (-mxxMzz);
    //////////////////////////////////////////////////////////////////////////
    m011 += omega * (-m011);
    m101 += omega * (-m101);
    m110 += omega * (-m110);

    ////////////////////////////////////////////////////////////////////////////////////
    // relax
    //////////////////////////////////////////////////////////////////////////
    // incl. limiter
    //! - Relaxation of third order cumulants including limiter according to Eq. (116)-(123)
    //! <a href="https://doi.org/10.1016/j.jcp.2017.05.040"><b>[ M. Geier et al. (2017),
    //! DOI:10.1016/j.jcp.2017.05.040 ]</b></a>
    //!
    Real wadjust = Oxyz + (c1o1 - Oxyz) * KERNEL_ABS(m111) / (KERNEL_ABS(m111) + quadricLimitD);
    m111 += wadjust * (-m111);
    wadjust = OxyyPxzz + (c1o1 - OxyyPxzz) * KERNEL_ABS(mxxyPyzz) / (KERNEL_ABS(mxxyPyzz) + quadricLimitP);
    mxxyPyzz += wadjust * (-mxxyPyzz);
    wadjust = OxyyMxzz + (c1o1 - OxyyMxzz) * KERNEL_ABS(mxxyMyzz) / (KERNEL_ABS(mxxyMyzz) + quadricLimitM);
    mxxyMyzz += wadjust * (-mxxyMyzz);
    wadjust = OxyyPxzz + (c1o1 - OxyyPxzz) * KERNEL_ABS(mxxzPyyz) / (KERNEL_ABS(mxxzPyyz) + quadricLimitP);
    mxxzPyyz += wadjust * (-mxxzPyyz);
    wadjust = OxyyMxzz + (c1o1 - OxyyMxzz) * KERNEL_ABS(mxxzMyyz) / (KERNEL_ABS(mxxzMyyz) + quadricLimitM);
    mxxzMyyz += wadjust * (-mxxzMyyz);
    wadjust = OxyyPxzz + (c1o1 - OxyyPxzz) * KERNEL_ABS(mxyyPxzz) / (KERNEL_ABS(mxyyPxzz) + quadricLimitP);
    mxyyPxzz += wadjust * (-mxyyPxzz);
    wadjust = OxyyMxzz + (c1o1 - OxyyMxzz) * KERNEL_ABS(mxyyMxzz) / (KERNEL_ABS(mxyyMxzz) + quadricLimitM);
    mxyyMxzz += wadjust * (-mxyyMxzz);
    //////////////////////////////////////////////////////////////////////////
    // no limiter
    // mfbbb += OxyyMxzz * (-mfbbb);
    // mxxyPyzz += OxyyPxzz * (-mxxyPyzz);
    // mxxyMyzz += OxyyMxzz * (-mxxyMyzz);
    // mxxzPyyz += OxyyPxzz * (-mxxzPyyz);
    // mxxzMyyz += OxyyMxzz * (-mxxzMyyz);
    // mxyyPxzz += OxyyPxzz * (-mxyyPxzz);
    // mxyyMxzz += OxyyMxzz * (-mxyyMxzz);

    ////////////////////////////////////////////////////////////////////////////////////
    //! - Compute inverse linear combinations of second and third order cumulants
    //!
    m200 = c1o3 * (mxxMyy + mxxMzz + mxxPyyPzz);
    m020 = c1o3 * (-c2o1 * mxxMyy + mxxMzz + mxxPyyPzz);
    m002 = c1o3 * (mxxMyy - c2o1 * mxxMzz + mxxPyyPzz);

    m210 = (mxxyMyzz + mxxyPyzz) * c1o2;
    m012 = (-mxxyMyzz + mxxyPyzz) * c1o2;
    m201 = (mxxzMyyz + mxxzPyyz) * c1o2;
    m021 = (-mxxzMyyz + mxxzPyyz) * c1o2;
    m120 = (mxyyMxzz + mxyyPxzz) * c1o2;
    m102 = (-mxyyMxzz + mxyyPxzz) * c1o2;
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // 4.
    // no limiter
    //! - Relax fourth order cumulants to modified equilibrium for fourth order convergence of diffusion according
    //! to Eq. (43)-(48) <a href="https://doi.org/10.1016/j.jcp.2017.05.040"><b>[ M. Geier et al. (2017),
    //! DOI:10.1016/j.jcp.2017.05.040 ]</b></a>
    //!
    cm022 = -O4 * (c1o1 / omega - c1o2) * (dyuy + dzuz) * c2o3 * factorA + (c1o1 - O4) * (cm022);
    cm202 = -O4 * (c1o1 / omega - c1o2) * (dxux + dzuz) * c2o3 * factorA + (c1o1 - O4) * (cm202);
    cm220 = -O4 * (c1o1 / omega - c1o2) * (dyuy + dxux) * c2o3 * factorA + (c1o1 - O4) * (cm220);
    cm112 = -O4 * (c1o1 / omega - c1o2) * Dxy * c1o3 * factorB + (c1o1 - O4) * (cm112);
    cm121 = -O4 * (c1o1 / omega - c1o2) * Dxz * c1o3 * factorB + (c1o1 - O4) * (cm121);
    cm211 = -O4 * (c1o1 / omega - c1o2) * Dyz * c1o3 * factorB + (c1o1 - O4) * (cm211);

    //////////////////////////////////////////////////////////////////////////
    // 5.
    cm122 += O5 * (-cm122);
    cm212 += O5 * (-cm212);
    cm221 += O5 * (-cm221);

    //////////////////////////////////////////////////////////////////////////
    // 6.
    cm222 += O6 * (-cm222);

    ////////////////////////////////////////////////////////////////////////////////////
    //! - Compute central moments from post collision cumulants according to Eq. (53)-(56) in
    //! <a href="https://doi.org/10.1016/j.jcp.2017.05.040"><b>[ M. Geier et al. (2017),
    //! DOI:10.1016/j.jcp.2017.05.040 ]</b></a>
    //!

    //////////////////////////////////////////////////////////////////////////
    // 4.
    m211 = cm211 + c1o3 * ((c3o1 * m200 + c1o1) * m011 + c6o1 * m110 * m101) * oneOverRho;
    m121 = cm121 + c1o3 * ((c3o1 * m020 + c1o1) * m101 + c6o1 * m110 * m011) * oneOverRho;
    m112 = cm112 + c1o3 * ((c3o1 * m002 + c1o1) * m110 + c6o1 * m101 * m011) * oneOverRho;

    m220 =
        cm220 + (((m200 * m020 + c2o1 * m110 * m110) * c9o1 + c3o1 * (m200 + m020)) * oneOverRho - (drho * oneOverRho)) * c1o9;
    m202 =
        cm202 + (((m200 * m002 + c2o1 * m101 * m101) * c9o1 + c3o1 * (m200 + m002)) * oneOverRho - (drho * oneOverRho)) * c1o9;
    m022 =
        cm022 + (((m002 * m020 + c2o1 * m011 * m011) * c9o1 + c3o1 * (m002 + m020)) * oneOverRho - (drho * oneOverRho)) * c1o9;

    //////////////////////////////////////////////////////////////////////////
    // 5.
    m122 = cm122 + c1o3 *
                       (c3o1 * (m002 * m120 + m020 * m102 + c4o1 * m011 * m111 + c2o1 * (m101 * m021 + m110 * m012)) +
                        (m120 + m102)) *
                       oneOverRho;
    m212 = cm212 + c1o3 *
                       (c3o1 * (m002 * m210 + m200 * m012 + c4o1 * m101 * m111 + c2o1 * (m011 * m201 + m110 * m102)) +
                        (m210 + m012)) *
                       oneOverRho;
    m221 = cm221 + c1o3 *
                       (c3o1 * (m200 * m021 + m020 * m201 + c4o1 * m110 * m111 + c2o1 * (m101 * m120 + m011 * m210)) +
                        (m021 + m201)) *
                       oneOverRho;

    //////////////////////////////////////////////////////////////////////////
    // 6.
    m222 = cm222 - ((-c4o1 * m111 * m111 - (m200 * m022 + m020 * m202 + m002 * m220) -
                     c4o1 * (m011 * m211 + m101 * m121 + m110 * m112) -
                     c2o1 * (m120 * m102 + m210 * m012 + m201 * m021)) *
                        oneOverRho +
                    (c4o1 * (m101 * m101 * m020 + m011 * m011 * m200 + m110 * m110 * m002) +
                     c2o1 * (m200 * m020 * m002) + c16o1 * m110 * m101 * m011) *
                        oneOverRho * oneOverRho -
                    c1o3 * (m022 + m202 + m220) * oneOverRho - c1o9 * (m200 + m020 + m002) * oneOverRho +
                    (c2o1 * (m101 * m101 + m011 * m011 + m110 * m110) +
                     (m002 * m020 + m002 * m200 + m020 * m200) + c1o3 * (m002 + m020 + m200)) *
                        oneOverRho * oneOverRho * c2o3 +
                    c1o27 * ((drho * drho - drho) * oneOverRho * oneOverRho));

    ////////////////////////////////////////////////////////////////////////////////////
    //! -  Add acceleration (body force) to first order cumulants according to Eq. (85)-(87) in
    //! <a href="https://doi.org/10.1016/j.camwa.2015.05.001"><b>[ M. Geier et al. (2015),
    //! DOI:10.1016/j.camwa.2015.05.001 ]</b></a>
    //!
    m100 = -m100;
    m010 = -m010;
    m001 = -m001;

    ////////////////////////////////////////////////////////////////////////////////////
    //! - Chimera transform from central moments to well conditioned distributions as defined in Appendix J in
    //! <a href="https://doi.org/10.1016/j.camwa.2015.05.001"><b>[ M. Geier et al. (2015),
    //! DOI:10.1016/j.camwa.2015.05.001 ]</b></a> see also Eq. (88)-(96) in <a
    //! href="https://doi.org/10.1016/j.jcp.2017.05.040"><b>[ M. Geier et al. (2017), DOI:10.1016/j.jcp.2017.05.040
    //! ]</b></a>
    //!
    ////////////////////////////////////////////////////////////////////////////////////
    // X - Dir
    backwardInverseChimeraWithK(m000, m100, m200, vvx, vx2, c1o1, c1o1);
    backwardChimera(m010, m110, m210, vvx, vx2);
    backwardInverseChimeraWithK(m020, m120, m220, vvx, vx2, c3o1, c1o3);
    backwardChimera(m001, m101, m201, vvx, vx2);
    backwardChimera(m011, m111, m211, vvx, vx2);
    backwardChimera(m021, m121, m221, vvx, vx2);
    backwardInverseChimeraWithK(m002, m102, m202, vvx, vx2, c3o1, c1o3);
    backwardChimera(m012, m112, m212, vvx, vx2);
    backwardInverseChimeraWithK(m022, m122, m222, vvx, vx2, c9o1, c1o9);

    ////////////////////////////////////////////////////////////////////////////////////
    // Y - Dir
    backwardInverseChimeraWithK(m000, m010, m020, vvy, vy2, c6o1, c1o6);
    backwardChimera(m001, m011, m021, vvy, vy2);
    backwardInverseChimeraWithK(m002, m012, m022, vvy, vy2, c18o1, c1o18);
    backwardInverseChimeraWithK(m100, m110, m120, vvy, vy2, c3o2, c2o3);
    backwardChimera(m101, m111, m121, vvy, vy2);
    backwardInverseChimeraWithK(m102, m112, m122, vvy, vy2, c9o2, c2o9);
    backwardInverseChimeraWithK(m200, m210, m220, vvy, vy2, c6o1, c1o6);
    backwardChimera(m201, m211, m221, vvy, vy2);
    backwardInverseChimeraWithK(m202, m212, m222, vvy, vy2, c18o1, c1o18);

    ////////////////////////////////////////////////////////////////////////////////////
    // Z - Dir
    backwardInverseChimeraWithK(m000, m001, m002, vvz, vz2, c36o1, c1o36);
    backwardInverseChimeraWithK(m010, m011, m012, vvz, vz2, c9o1, c1o9);
    backwardInverseChimeraWithK(m020, m021, m022, vvz, vz2, c36o1, c1o36);
    backwardInverseChimeraWithK(m100, m101, m102, vvz, vz2, c9o1, c1o9);
    backwardInverseChimeraWithK(m110, m111, m112, vvz, vz2, c9o4, c4o9);
    backwardInverseChimeraWithK(m120, m121, m122, vvz, vz2, c9o1, c1o9);
    backwardInverseChimeraWithK(m200, m201, m202, vvz, vz2, c36o1, c1o36);
    backwardInverseChimeraWithK(m210, m211, m212, vvz, vz2, c9o1, c1o9);
    backwardInverseChimeraWithK(m220, m221, m222, vvz, vz2, c36o1, c1o36);

    // 分布函数回代
    fold(i, j, k, 1) = fP00;
    fold(i, j, k, 2) = fM00;
    fold(i, j, k, 3) = f0P0;
    fold(i, j, k, 4) = f0M0;
    fold(i, j, k, 5) = f00P;
    fold(i, j, k, 6) = f00M;
    fold(i, j, k, 7) = fPP0;
    fold(i, j, k, 8) = fMM0;
    fold(i, j, k, 9) = fPM0;
    fold(i, j, k, 10) = fMP0;
    fold(i, j, k, 11) = fP0P;
    fold(i, j, k, 12) = fM0M;
    fold(i, j, k, 13) = fP0M;
    fold(i, j, k, 14) = fM0P;
    fold(i, j, k, 15) = f0PP;
    fold(i, j, k, 16) = f0MM;
    fold(i, j, k, 17) = f0PM;
    fold(i, j, k, 18) = f0MP;
    fold(i, j, k, 0) = f000;
    fold(i, j, k, 19) = fPPP;
    fold(i, j, k, 21) = fPMP;
    fold(i, j, k, 23) = fPPM;
    fold(i, j, k, 25) = fPMM;
    fold(i, j, k, 20) = fMPP;
    fold(i, j, k, 22) = fMMP;
    fold(i, j, k, 24) = fMPM;
    fold(i, j, k, 26) = fMMM;
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void stream(int i, int j, int k, amrex::Array4<amrex::Real> const& fold, amrex::Array4<amrex::Real> const& fnew,
            amrex::IntVect const& hi, const bool is_finest) {
    for (int q = 0; q < Q; q++) {
        int xm = i - e[q][0];
        int ym = j - e[q][1];
        int zm = k - e[q][2];
        // 这里的设置需要跟main函数中的{AMREX_D_DECL(1, 0, 1)}的周期性设置相对应
        if ((i >= 0) && (i <= hi[0]) && (j > 0) && (j < hi[1]) && (k >= 0) && (k <= hi[2])) {
            fnew(i, j, k, q) = fold(xm, ym, zm, q);
        }
    }
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void swap_ddf(int i, int j, int k, amrex::Array4<amrex::Real> const& fold,
              amrex::Array4<amrex::Real> const& fnew) {
    for (int q = 0; q < Q; q++) {
        fold(i, j, k, q) = fnew(i, j, k, q);
    }
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void interp_scale(int i, int j, int k, amrex::Array4<amrex::Real> const& fold,
                  amrex::Array4<amrex::Real> const& fnew, amrex::Real scale) {
    Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0, res = 0.0;

    for (int q = 0; q < Q; q++) {
        ft = fold(i, j, k, q); // 这里的f就是压力分布函数
        rhot += ft;
        uxt += ft * e[q][0];
        uyt += ft * e[q][1];
        uzt += ft * e[q][2];
    }

    uxt = uxt / rhot;
    uyt = uyt / rhot;
    uzt = uzt / rhot;

    for (int q = 0; q < Q; q++) {
        res = feqQian(rhot, {uxt, uyt, uzt}, q);
        fnew(i, j, k, q) = res + (fold(i, j, k, q) - res) * scale;
    }
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void average_scale(int i, int j, int k, amrex::Array4<amrex::Real> const& fold, amrex::Real scale) {

    Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0, res = 0.0;

    for (int q = 0; q < Q; q++) {
        ft = fold(i, j, k, q);
        rhot += ft;
        uxt += ft * e[q][0];
        uyt += ft * e[q][1];
        uzt += ft * e[q][2];
    }

    uxt = uxt / rhot;
    uyt = uyt / rhot;
    uzt = uzt / rhot;

    for (int q = 0; q < Q; q++) {
        res = feqQian(rhot, {uxt, uyt, uzt}, q);
        fold(i, j, k, q) = res + (fold(i, j, k, q) - res) * scale;
    }
}

struct AmrCoreFill {
    AMREX_GPU_DEVICE
    void operator()(const amrex::IntVect& /*iv*/, amrex::Array4<amrex::Real> const& /*data*/,
                    const int /*dcomp*/, const int /*numcomp*/,
                    amrex::GeometryData const& /*geom*/, const amrex::Real /*time*/,
                    const amrex::BCRec* /*bcr*/, const int /*bcomp*/,
                    const int /*orig_comp*/) const {
        // do something for external Dirichlet (BCType::ext_dir)
    }
};

AMREX_GPU_HOST_DEVICE // 自适应加密
    AMREX_FORCE_INLINE void
    state_error(int i, int j, int k,
                amrex::Array4<char> const& tag,
                amrex::Array4<amrex::Real const> const& vort,
                amrex::Real phierr, char tagval, char clearval, int lev, amrex::Real dx,
                amrex::IntVect const& lo2, amrex::IntVect const& hi2, amrex::RealVect const& pos) {
    Real x_r = pos[0] * dx_0 / dx;
    Real y_r = pos[1] * dx_0 / dx;
    Real z_r = pos[2] * dx_0 / dx;

    Real r_r = R * dx_0 / dx;

    Real dist = sqrt((i - x_r) * (i - x_r) + (j - y_r) * (j - y_r) + (k - z_r) * (k - z_r));

    // 根据固体表面距离加密
    if (dist <= r_r + 32) {
        tag(i, j, k) = tagval;
    }

    // 网格自适应加密
    // if(vort(i, j, k, 0) >= 0.00001 && vort(i, j, k, 0) <= 0.000011)
    // {
    //     tag(i,j,k) = tagval;
    // }

    // 触碰边界处加密清除
    if (i <= lo2[0] || i >= hi2[0] || j <= lo2[1] || j >= hi2[1] || k <= lo2[2] || k >= hi2[2]) {
        tag(i, j, k) = clearval;
    }
}

AMREX_GPU_HOST_DEVICE // 自适应加密
    AMREX_FORCE_INLINE void
    state_error_2(int i, int j, int k,
                  amrex::Array4<char> const& tag,
                  amrex::Array4<amrex::Real const> const& vort,
                  amrex::Real phierr, char tagval, char clearval, int lev, amrex::GeometryData const& geomdata,
                  amrex::IntVect const& lo2, amrex::IntVect const& hi2, amrex::RealVect const& pos) {
    const amrex::Real* dx = geomdata.CellSize();

    Real x_r = pos[0] * dx_0 / dx[0];
    Real y_r = pos[1] * dx_0 / dx[1];
    Real z_r = pos[2] * dx_0 / dx[2];

    Real r_r = R * dx_0 / dx[0];

    Real dist = sqrt((i - x_r) * (i - x_r) + (j - y_r) * (j - y_r) + (k - z_r) * (k - z_r));

    // 根据固体表面距离加密
    if (dist <= r_r + 1 && dist >= r_r - 1) {
        tag(i, j, k) = tagval;
    }

    if (i >= lo2[0] && i <= hi2[0] && j >= lo2[1] && j <= hi2[1] && k >= lo2[2] && k <= hi2[2]) {
        tag(i, j, k) = tagval;
    }
    // 网格自适应加密
    // if(vort(i, j, k, 0) >= 0.00001 && vort(i, j, k, 0) <= 0.000011)
    // {
    //     tag(i,j,k) = tagval;
    // }

    // if(i <= 10 || i >= geomdata.Domain().length(0)-10
    // || j <= 10 || j >= geomdata.Domain().length(1)-10
    // || k <= 10 || k >= geomdata.Domain().length(2)-10
    // )
    // {
    //     tag(i,j,k) = tagval;
    // }
}

AMREX_GPU_HOST_DEVICE // 用于多颗粒根据圆心加密
    AMREX_FORCE_INLINE void
    state_error_3(int i, int j, int k,
                  amrex::Array4<char> const& tag,
                  amrex::Array4<amrex::Real const> const& vort,
                  amrex::Real phierr, char tagval, char clearval, int lev, amrex::GeometryData const& geomdata,
                  amrex::IntVect const& lo2, amrex::IntVect const& hi2, amrex::RealVect* p_pos, const int particle_num) {
    const amrex::Real* dx = geomdata.CellSize();

    // 根据固体表面距离加密
    for (int p_num = 0; p_num < particle_num; p_num++) {
        Real x_r = p_pos[p_num][0] * dx_0 / dx[0];
        Real y_r = p_pos[p_num][1] * dx_0 / dx[0];
        Real z_r = p_pos[p_num][2] * dx_0 / dx[0];

        Real r_r = R * dx_0 / dx[0];

        Real dist = sqrt((i - x_r) * (i - x_r) + (j - y_r) * (j - y_r) + (k - z_r) * (k - z_r));

        if (dist <= r_r + 3 && dist >= r_r - 1) {
            tag(i, j, k) = tagval;
        }
    }

    // if(i >= lo2[0] && i <= hi2[0] && j >= lo2[1] && j <= hi2[1] && k >= lo2[2] && k <= hi2[2])
    // {
    //     tag(i,j,k) = tagval;
    // }

    // 网格自适应加密
    // if(vort(i, j, k, 0) >= 0.00001 && vort(i, j, k, 0) <= 0.000011)
    // {
    //     tag(i,j,k) = tagval;
    // }

    // if(i <= 10 || i >= geomdata.Domain().length(0)-10
    // || j <= 10 || j >= geomdata.Domain().length(1)-10
    // || k <= 10 || k >= geomdata.Domain().length(2)-10
    // )
    // {
    //     tag(i,j,k) = tagval;
    // }
}

AMREX_GPU_HOST_DEVICE // 用于多复杂结构体固定区域加密
    AMREX_FORCE_INLINE void
    state_error_4(int i, int j, int k,
                  amrex::Array4<char> const& tag,
                  amrex::Array4<amrex::Real const> const& vort,
                  amrex::Real phierr, char tagval, char clearval, int lev, amrex::GeometryData const& geomdata,
                  amrex::IntVect const& lo2, amrex::IntVect const& hi2, amrex::RealVect* p_pos, const int particle_num) {
    const amrex::Real* dx = geomdata.CellSize();

    // 根据固体表面距离加密
    // for(int p_num = 0; p_num < particle_num; p_num++)
    // {
    //     Real x_r = p_pos[p_num][0] * dx_0 / dx[0];
    //     Real y_r = p_pos[p_num][1] * dx_0 / dx[0];
    //     Real z_r = p_pos[p_num][2] * dx_0 / dx[0];

    //     Real r_r = R * dx_0 / dx[0];

    //     Real dist = sqrt((i - x_r)*(i - x_r) + (j - y_r)*(j - y_r) + (k - z_r)*(k - z_r));

    //     if(dist <= r_r+3 && dist >= r_r-1)
    //     {
    //         tag(i,j,k) = tagval;
    //     }
    // }

    if (i >= lo2[0] && i <= hi2[0] && j >= lo2[1] && j <= hi2[1] && k >= lo2[2] && k <= hi2[2]) {
        tag(i, j, k) = tagval;
    }

    // 网格自适应加密
    // if(vort(i, j, k, 0) >= 0.00001 && vort(i, j, k, 0) <= 0.000011)
    // {
    //     tag(i,j,k) = tagval;
    // }

    // if(i <= 10 || i >= geomdata.Domain().length(0)-10
    // || j <= 10 || j >= geomdata.Domain().length(1)-10
    // || k <= 10 || k >= geomdata.Domain().length(2)-10
    // )
    // {
    //     tag(i,j,k) = tagval;
    // }
}

AMREX_GPU_HOST_DEVICE // 用于多圆球区域加密
    AMREX_FORCE_INLINE void
    state_error_5(int i, int j, int k,
                  amrex::Array4<char> const& tag,
                  amrex::Array4<amrex::Real const> const& vort,
                  amrex::Real phierr, char tagval, char clearval, int lev, amrex::GeometryData const& geomdata,
                  amrex::IntVect const& lo2, amrex::IntVect const& hi2, amrex::RealVect* p_pos, const int particle_num) {
    const amrex::Real* dx = geomdata.CellSize();

    // 根据固体表面距离加密
    for (int p_num = 0; p_num < particle_num; p_num++) {
        Real x_r = p_pos[p_num][0] * dx_0 / dx[0];
        Real y_r = p_pos[p_num][1] * dx_0 / dx[0];
        Real z_r = p_pos[p_num][2] * dx_0 / dx[0];

        Real r_r = R * dx_0 / dx[0];
        Real d_r = 2 * r_r;
        Real dist = sqrt((i - x_r) * (i - x_r) + (j - y_r) * (j - y_r) + (k - z_r) * (k - z_r));
        Real x_lo, x_hi, y_lo, y_hi, z_lo, z_hi;

        if (lev == 0) {
            x_lo = (x_r - 1.5 * d_r);
            y_lo = (y_r - 2.0 * d_r);
            z_lo = (z_r - 2.0 * d_r);

            x_hi = (x_r + 5.0 * d_r);
            y_hi = (y_r + 2.0 * d_r);
            z_hi = (z_r + 2.0 * d_r);

            if (i >= x_lo && i <= x_hi && j >= y_lo && j <= y_hi && k >= z_lo && k <= z_hi && dist >= r_r - 1) {
                tag(i, j, k) = tagval;
            }
        }
        if (lev == 1) {
            x_lo = (x_r - 1.2 * d_r);
            y_lo = (y_r - 1.5 * d_r);
            z_lo = (z_r - 1.5 * d_r);

            x_hi = (x_r + 4.0 * d_r);
            y_hi = (y_r + 1.5 * d_r);
            z_hi = (z_r + 1.5 * d_r);

            if (i >= x_lo && i <= x_hi && j >= y_lo && j <= y_hi && k >= z_lo && k <= z_hi && dist >= r_r - 1) {
                tag(i, j, k) = tagval;
            }
        }
        if (lev == 2) {
            x_lo = (x_r - 1.0 * d_r);
            y_lo = (y_r - 1.0 * d_r);
            z_lo = (z_r - 1.0 * d_r);

            x_hi = (x_r + 2.5 * d_r);
            y_hi = (y_r + 1.0 * d_r);
            z_hi = (z_r + 1.0 * d_r);

            if (i >= x_lo && i <= x_hi && j >= y_lo && j <= y_hi && k >= z_lo && k <= z_hi && dist >= r_r - 1) {
                tag(i, j, k) = tagval;
            }
        }
        if (lev == 3) {
            x_lo = (x_r - 0.7 * d_r);
            y_lo = (y_r - 0.8 * d_r);
            z_lo = (z_r - 0.8 * d_r);

            x_hi = (x_r + 1.5 * d_r);
            y_hi = (y_r + 0.8 * d_r);
            z_hi = (z_r + 0.8 * d_r);

            if (dist >= r_r - 1 && dist <= r_r + 1) {
                tag(i, j, k) = tagval;
            }
        }
        if (lev == 4) {
            if (dist >= r_r - 1 && dist <= r_r + 1) {
                tag(i, j, k) = tagval;
            }
        }
    }
}

//********************************************************************//
//                         IBM   function                             //
//********************************************************************//
AMREX_GPU_HOST_DEVICE
AMREX_FORCE_INLINE
Real delta3p(amrex::Real r) {
    r = fabs(r);
    if (r <= 0.5)
        return ((1.0 + sqrt(1.0 - 3.0 * r * r)) / 3.0);
    else if (r <= 1.5)
        return ((5.0 - 3.0 * r - sqrt(1.0 - 3.0 * (1.0 - r) * (1.0 - r))) / 6.0);
    else
        return 0.0;
}

template <typename P>
AMREX_GPU_HOST_DEVICE
    AMREX_FORCE_INLINE void
    force_interp_extrap(P const& p,
                        amrex::Real& fx, amrex::Real& fy, amrex::Real& fz,
                        amrex::Real& tx, amrex::Real& ty, amrex::Real& tz,
                        amrex::Real& x_local, amrex::Real& y_local, amrex::Real& z_local,
                        amrex::Real& area,
                        amrex::Array4<amrex::Real> const& u, amrex::Array4<amrex::Real> const& rho, amrex::Array4<amrex::Real> const& Ft,
                        amrex::Real const delta, amrex::RealVect const& uc, amrex::RealVect const& wc, amrex::RealVect const& centre) {
    amrex::Real xt, yt, zt, lx, ly, lz;
    amrex::Real IB_Interp, IB_weight;
    amrex::Real uxt, uyt, uzt, rhot, upx, upy, upz, fxt, fyt, fzt;

    xt = p.pos(0) / delta;
    yt = p.pos(1) / delta;
    zt = p.pos(2) / delta;

    IB_weight = LP_dr * area;

    uxt = 0.0;
    uyt = 0.0;
    uzt = 0.0;
    rhot = 0.0;

    upx = uc[0] + wc[2] * (-y_local) + wc[1] * (z_local);
    upy = uc[1] + wc[2] * (x_local) + wc[0] * (-z_local);
    upz = uc[2] + wc[0] * (y_local) + wc[1] * (-x_local);

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            for (int z = -2; z <= 2; z++) {
                int xx = static_cast<int>(amrex::Math::floor(xt)) + x; // 拉格朗日点作用范围内真实的点坐标
                int yy = static_cast<int>(amrex::Math::floor(yt)) + y;
                int zz = static_cast<int>(amrex::Math::floor(zt)) + z;

                lx = xt - (xx + 0.5);
                ly = yt - (yy + 0.5);
                lz = zt - (zz + 0.5);

                IB_Interp = delta3p(lx) * delta3p(ly) * delta3p(lz);

                uxt += u(xx, yy, zz, 0) * IB_Interp;
                uyt += u(xx, yy, zz, 1) * IB_Interp;
                uzt += u(xx, yy, zz, 2) * IB_Interp;
                rhot += rho(xx, yy, zz, 0) * IB_Interp;
            }
        }
    }

    rhot = 2 * rhot / dt_min;

    fxt = rhot * (uxt - upx);
    fyt = rhot * (uyt - upy);
    fzt = rhot * (uzt - upz);

    fx = fxt * IB_weight * dx_min * dx_min * dx_min;
    fy = fyt * IB_weight * dx_min * dx_min * dx_min;
    fz = fzt * IB_weight * dx_min * dx_min * dx_min;

    tx = (fzt * y_local - fyt * z_local) * IB_weight * dx_min * dx_min * dx_min;
    ty = (fxt * z_local - fzt * x_local) * IB_weight * dx_min * dx_min * dx_min;
    tz = (fyt * x_local - fxt * y_local) * IB_weight * dx_min * dx_min * dx_min;

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            for (int z = -2; z <= 2; z++) {
                int xx = static_cast<int>(amrex::Math::floor(xt)) + x;
                int yy = static_cast<int>(amrex::Math::floor(yt)) + y;
                int zz = static_cast<int>(amrex::Math::floor(zt)) + z;

                lx = xt - (xx + 0.5);
                ly = yt - (yy + 0.5);
                lz = zt - (zz + 0.5);

                IB_Interp = delta3p(lx) * delta3p(ly) * delta3p(lz) * IB_weight;

                amrex::Gpu::Atomic::AddNoRet(&Ft(xx, yy, zz, 0), -fxt * IB_Interp);
                amrex::Gpu::Atomic::AddNoRet(&Ft(xx, yy, zz, 1), -fyt * IB_Interp);
                amrex::Gpu::Atomic::AddNoRet(&Ft(xx, yy, zz, 2), -fzt * IB_Interp);
            }
        }
    }
}

template <typename P>
AMREX_GPU_HOST_DEVICE
    AMREX_FORCE_INLINE void
    force_wall_model(P const& p,
                     amrex::Real& fx, amrex::Real& fy, amrex::Real& fz,
                     amrex::Real& tx, amrex::Real& ty, amrex::Real& tz,
                     amrex::Real& x_local, amrex::Real& y_local, amrex::Real& z_local,
                     amrex::Real& area,
                     amrex::Array4<amrex::Real> const& u, amrex::Array4<amrex::Real> const& rho, amrex::Array4<amrex::Real> const& Ft,
                     amrex::Real const delta, amrex::RealVect const& uc, amrex::RealVect const& wc, amrex::RealVect const& centre) {
    amrex::Real xc, yc, zc, lx, ly, lz;
    amrex::Real IB_Interp, IB_weight;
    amrex::Real xq, yq, zq, xRef, yRef, zRef;
    amrex::Real uxRef, uyRef, uzRef, rhoRef;
    amrex::Real fxt, fyt, fzt;

    xc = centre[0] / delta;
    yc = centre[1] / delta;
    zc = centre[2] / delta;

    IB_weight = LP_dr * area;

    // 找到Ref和Q的坐标
    amrex::Real distSum = std::sqrt(x_local * x_local + y_local * y_local + z_local * z_local);
    amrex::RealVect normal = {x_local / distSum, y_local / distSum, z_local / distSum};

    xq = xc + x_local / delta + normal[0] * 0.25;
    yq = yc + y_local / delta + normal[1] * 0.25;
    zq = zc + z_local / delta + normal[2] * 0.25;

    xRef = xc + x_local / delta + normal[0] * 2.5;
    yRef = yc + y_local / delta + normal[1] * 2.5;
    zRef = zc + z_local / delta + normal[2] * 2.5;

    // 插值得到Ref点的速度，密度
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            for (int z = -2; z <= 2; z++) {
                int xx = static_cast<int>(amrex::Math::floor(xRef)) + x; // 拉格朗日点作用范围内真实的点坐标
                int yy = static_cast<int>(amrex::Math::floor(yRef)) + y;
                int zz = static_cast<int>(amrex::Math::floor(zRef)) + z;

                lx = xRef - (xx + 0.5);
                ly = yRef - (yy + 0.5);
                lz = zRef - (zz + 0.5);

                IB_Interp = delta3p(lx) * delta3p(ly) * delta3p(lz);

                uxRef += u(xx, yy, zz, 0) * IB_Interp;
                uyRef += u(xx, yy, zz, 1) * IB_Interp;
                uzRef += u(xx, yy, zz, 2) * IB_Interp;
                rhoRef += rho(xx, yy, zz, 0) * IB_Interp;
            }
        }
    }

    // 找到切向
    amrex::Real dotProduct = uxRef * normal[0] + uyRef * normal[1] + uzRef * normal[2];
    amrex::Real vTangX = uxRef - dotProduct * normal[0];
    amrex::Real vTangY = uyRef - dotProduct * normal[1];
    amrex::Real vTangZ = uzRef - dotProduct * normal[2];
    amrex::Real vTangSum = std::sqrt(vTangX * vTangX + vTangY * vTangY + vTangZ * vTangZ);
    amrex::RealVect tangent = {vTangX / vTangSum, vTangY / vTangSum, vTangZ / vTangSum};

    // 幂定律计算tw
    amrex::Real velNormal = std::abs(dotProduct);
    amrex::Real velTang = vTangSum;

    amrex::Real tw = rhoRef * std::pow((velTang / 8.3), (2 / (1.0 + 1.0 / 7.0))) * std::pow((mv_0 / 2.5), (2.0 / 7.0 / (1.0 + 1.0 / 7.0)));

    // 计算Q点剪切应力和体积力
    rhoRef = 2 * rhoRef / dt_min;
    amrex::Real fTang = 2.0 * tw / dx_min;
    amrex::Real fNormal = rhoRef * (velNormal - 0.0);

    // 力外推
    fxt = fNormal * normal[0] + fTang * tangent[0];
    fyt = fNormal * normal[1] + fTang * tangent[1];
    fzt = fNormal * normal[2] + fTang * tangent[2];
    fx = fxt * IB_weight * dx_min * dx_min * dx_min;
    fy = fyt * IB_weight * dx_min * dx_min * dx_min;
    fz = fzt * IB_weight * dx_min * dx_min * dx_min;

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            for (int z = -2; z <= 2; z++) {
                int xx = static_cast<int>(amrex::Math::floor(xq)) + x;
                int yy = static_cast<int>(amrex::Math::floor(yq)) + y;
                int zz = static_cast<int>(amrex::Math::floor(zq)) + z;

                lx = xq - (xx + 0.5);
                ly = yq - (yy + 0.5);
                lz = zq - (zz + 0.5);

                IB_Interp = delta3p(lx) * delta3p(ly) * delta3p(lz) * IB_weight;

                amrex::Gpu::Atomic::AddNoRet(&Ft(xx, yy, zz, 0), -fxt * IB_Interp);
                amrex::Gpu::Atomic::AddNoRet(&Ft(xx, yy, zz, 1), -fyt * IB_Interp);
                amrex::Gpu::Atomic::AddNoRet(&Ft(xx, yy, zz, 2), -fzt * IB_Interp);
            }
        }
    }
}

template <typename P>
AMREX_GPU_HOST_DEVICE
    AMREX_FORCE_INLINE void
    pressure_interp(P const& p, amrex::Real& cp, amrex::Array4<amrex::Real> const& rho, amrex::Real const delta) {
    amrex::Real xt, yt, zt, lx, ly, lz;
    amrex::Real IB_Interp;
    amrex::Real rhot;

    xt = p.pos(0) / delta;
    yt = p.pos(1) / delta;
    zt = p.pos(2) / delta;

    rhot = 0.0;

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            for (int z = -2; z <= 2; z++) {
                int xx = static_cast<int>(amrex::Math::floor(xt)) + x;
                int yy = static_cast<int>(amrex::Math::floor(yt)) + y;
                int zz = static_cast<int>(amrex::Math::floor(zt)) + z;

                lx = xt - (xx + 0.5);
                ly = yt - (yy + 0.5);
                lz = zt - (zz + 0.5);

                IB_Interp = delta3p(lx) * delta3p(ly) * delta3p(lz);

                rhot += rho(xx, yy, zz, 0) * IB_Interp;
            }
        }
    }

    amrex::Real rhotp = rhot * cs2;
    amrex::Real m = 0.5 * rho0 * U0 * U0;
    cp = (rhotp - p0) / m;
}

#endif