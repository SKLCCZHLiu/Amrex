#ifndef D2Q9_H_
#define D2Q9_H_

#include <AMReX_Array4.H>

#define PI (3.141592653589793238462643383279)

using namespace amrex;

#define DIM 2
#define Q 9

const int coarsest_level = 0;
const int max_ref_level = 3;
const int rate = (1 << max_ref_level);

const Real D = 20.0;
const Real R = D / 2.0;

const int NX = 40 * D;
const int NY = 40 * D;
const int NZ = 40 * D;

// const int NX = 1024;
// const int NY = 1024;
// const int NZ = 1024;

const Real nx = 1.0;
const Real ny = 1.0;
const Real nz = 1.0;

#define Re 200
#define dx_0 (nx / NX)
#define dt_0 dx_0
#define c 1.0
#define cs (c / sqrt(3.0))
#define cs2 (cs * cs)
#define p0 1.0
#define rho0 (p0 / cs2)
#define Ma (0.03 * sqrt(3.0))
#define U0 (Ma * cs)
#define mv_0 (U0 * D * dx_0 / Re)
#define tau_0 (mv_0 / (dt_0 * cs2) + 0.5)

const Real X = NX / 2.0;
const Real Y = NY / 2.0;
const Real Z = NZ / 2.0;
const Real center[DIM] = {X, Y};

// 全都转成真实大小
const Real dx_min = dx_0 / rate;
const Real dt_min = dt_0 / rate;

const Real dR = 0.0;                     // 内缩距离
const Real Rm = R - dR;                  // 等效半径
const Real ds0 = 1.0 * dx_min;           // 颗粒表面拉氏点的距离初设值
const int np = 2 * PI * Rm * dx_0 / ds0; // 拉格朗日点数量
const Real drs = 1.9;                    // 固体边界厚度，DF为1.0，3p函数为1.95,4p函数为2.6
const Real ds_r = 2 * PI * Rm / np * dx_0 / dx_min;
const Real IB_weight = ds_r * drs;

AMREX_GPU_DEVICE AMREX_FORCE_INLINE
    IntVect e[Q] =
        {
            {0, 0},
            {1, 0},
            {0, 1},
            {-1, 0},
            {0, -1},
            {1, 1},
            {-1, 1},
            {-1, -1},
            {1, -1}};

AMREX_GPU_DEVICE AMREX_FORCE_INLINE
    Real w[Q] =
        {
            4. / 9.,
            1. / 9.,
            1. / 9.,
            1. / 9.,
            1. / 9.,
            1. / 36.,
            1. / 36.,
            1. / 36.,
            1. / 36.};

AMREX_GPU_DEVICE AMREX_FORCE_INLINE
    Real
    feqHeLuo(const Real& pressure, const RealVect& u, const int i) {
    amrex::Real eu, uv, feqt;

    eu = e[i][0] * u[0] + e[i][1] * u[1];

    uv = (u[0] * u[0] + u[1] * u[1]);

    feqt = w[i] * (pressure + p0 * (3.0 * eu + 4.5 * eu * eu - 1.5 * uv));

    return feqt;
}

AMREX_GPU_DEVICE AMREX_FORCE_INLINE
    Real
    forceGuo(const RealVect& u, const RealVect& Ft, const int i) // 怎么会出问题，太离谱了？
{
    amrex::Real m, n, nf;

    m = ((e[i][0] - u[0]) * Ft[0] + (e[i][1] - u[1]) * Ft[1]) / cs2;

    n = (e[i][0] * u[0] + e[i][1] * u[1]) / cs2;

    nf = (e[i][0] * Ft[0] + e[i][1] * Ft[1]) / cs2;

    return w[i] * (m + n * nf);
}

#endif