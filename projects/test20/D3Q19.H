#ifndef D3Q19_H_
#define D3Q19_H_

#include <AMReX_Array4.H>

#define PI (3.141592653589793238462643383279)

using namespace amrex;

#define DIM 3
#define Q 27

const int coarsest_level = 0;
const int max_ref_level = 0;
const int rate = (1 << max_ref_level);

/*-----------------复杂结构体------------------------*/
// const Real obj_lo_x = -16.0;
// const Real obj_hi_x = 16.0;

// const Real obj_lo_y = -67.0;
// const Real obj_hi_y = 5.0;

// const Real obj_lo_z = -4.0;
// const Real obj_hi_z = 20.0;

// const Real obj_dim_x = (obj_hi_x  - obj_lo_x);
// const Real obj_dim_y = (obj_hi_y  - obj_lo_y);
// const Real obj_dim_z = (obj_hi_z  - obj_lo_z);
/*---------------------------------------------------*/

const Real D = 16.0; // obj_dim_z;
const Real R = D / 2.0;

constexpr int NX = 1024;
constexpr int NY = 256;
constexpr int NZ = 256;

const Real nx = NX;
const Real ny = NY;
const Real nz = NZ;

#define dx_0 (nx / NX)
#define dt_0 (dx_0)
#define c (1.0)
#define cs (c / sqrt(3.0))
#define cs2 (cs * cs)
#define rho0 (1.0) //(p0 / cs2)
#define p0 (rho0 * cs2)
#define Ma (0.03 * sqrt(3.0))

/*---------槽道流--------------------------*/
#define Del (NZ / 2.0)
#define Ret (180)
#define Uc (0.1)
#define Ut (0.00526804)
#define mv_0 (Ut * Del / Ret)
#define tau_0 (mv_0 / (cs2) + 0.5)

#define G (981)
#define Re (Uc * Del / mv_0)
#define U0 (Uc)
/*---------圆球绕流--------------------------*/
// #define Del   (NZ/2.0)
// #define Re    (3300)
// #define Ret   (180)
// #define U0    (0.1)
// #define mv_0  (U0 * Del * dx_0 / Re)
// #define Ut    (mv_0 * Ret / Del)
// #define tau_0 (mv_0 / (cs2 * dt_0) + 0.5)
// #define G     (981)
/*-----------颗粒流--------------------------*/
// #define Re    (1.5)
// #define U0    (1.04375) //(Ma * cs)
// #define mv_0  (0.004589623)
// #define tau_0 (mv_0 / (cs2 * dt_0) + 0.5)
// #define G     (0.001878036)
/*-------------------------------------------*/

#define rhof (rho0)
#define rhop (1.14 * rho0)
#define volume (PI * D * dx_0 * D * dx_0 * D * dx_0 / 6.0)
#define Mp (rhop * volume) // g
#define Mf (rhof * volume) // g
#define Mp_iner (Mp * D * dx_0 * D * dx_0 / 10.0)
#define Mf_iner (Mf * D * dx_0 * D * dx_0 / 10.0)

const Real X = 6.0 * D; // NX / 2.0;
const Real Y = NY / 2.0;
const Real Z = NZ / 2.0;
const Real center[DIM] = {X, Y, Z};

// 颗粒相关的参数
const Real dx_min = dx_0 / rate;
const Real dt_min = dt_0 / rate;

const Real rb = pow(((pow(rate * R, 3) + pow(rate * R - 1, 3)) / 2.0), (1.0 / 3.0)); // 修正的半径

const Real ds0 = 1.0;
const int ns = PI * rb / ds0 + 1;  // 表面分带数
const int nt1 = 2 * PI * rb / ds0; // 赤道上point数量

const Real LP_dr = 1.0;
const Real LP_area = 4 * PI * rb * rb;

// 多颗粒相关的参数
const Real safe = 0.025 * R;
const Real Epp1 = 0.25;
const Real Epp2 = 0.02;
const Real Epw1 = 0.125;
const Real Epw2 = 0.01;

// D3Q19
//  AMREX_GPU_DEVICE AMREX_FORCE_INLINE
//  IntVect e[Q] =
//  {
//      {  0,  0,  0 },
//      {  1,  0,  0 },
//      { -1,  0,  0 },
//      {  0,  1,  0 },
//      {  0, -1,  0 },
//      {  0,  0,  1 },
//      {  0,  0, -1 },
//      {  1,  1,  0 },
//      { -1, -1,  0 },
//      {  1, -1,  0 },
//      { -1,  1,  0 },
//      {  1,  0,  1 },
//      { -1,  0, -1 },
//      {  1,  0, -1 },
//      { -1,  0,  1 },
//      {  0,  1,  1 },
//      {  0, -1, -1 },
//      {  0,  1, -1 },
//      {  0, -1,  1 }
//  };

// AMREX_GPU_DEVICE AMREX_FORCE_INLINE
// Real w[Q] =
// {
//     1./3.,
//     1./18.,
//     1./18.,
//     1./18.,
//     1./18.,
//     1./18.,
//     1./18.,
//     1./36.,
//     1./36.,
//     1./36.,
//     1./36.,
//     1./36.,
//     1./36.,
//     1./36.,
//     1./36.,
//     1./36.,
//     1./36.,
//     1./36.,
//     1./36.
// };

// D3Q27_CUMULANT
AMREX_GPU_DEVICE AMREX_FORCE_INLINE
    IntVect e[Q] =
        {
            {0, 0, 0},
            {0, 1, 0},
            {0, -1, 0},
            {-1, 0, 0},
            {1, 0, 0},
            {0, 0, 1},
            {0, 0, -1},
            {-1, 1, 0},
            {1, 1, 0},
            {-1, -1, 0},
            {1, -1, 0},
            {0, 1, 1},
            {0, -1, 1},
            {-1, 0, 1},
            {1, 0, 1},
            {0, 1, -1},
            {0, -1, -1},
            {-1, 0, -1},
            {1, 0, -1},
            {1, 1, 1},
            {-1, 1, 1},
            {1, -1, 1},
            {-1, -1, 1},
            {1, 1, -1},
            {-1, 1, -1},
            {1, -1, -1},
            {-1, -1, -1}};

AMREX_GPU_DEVICE AMREX_FORCE_INLINE
    Real w[Q] =
        {
            8. / 27.,
            2. / 27.,
            2. / 27.,
            2. / 27.,
            2. / 27.,
            2. / 27.,
            2. / 27.,
            1. / 54.,
            1. / 54.,
            1. / 54.,
            1. / 54.,
            1. / 54.,
            1. / 54.,
            1. / 54.,
            1. / 54.,
            1. / 54.,
            1. / 54.,
            1. / 54.,
            1. / 54.,
            1. / 216.,
            1. / 216.,
            1. / 216.,
            1. / 216.,
            1. / 216.,
            1. / 216.,
            1. / 216.,
            1. / 216.};

// D3Q27_CUMULANT_OPT
//  AMREX_GPU_DEVICE AMREX_FORCE_INLINE
//  IntVect e[Q] =
//  {
//      {  0,  0,  0 },
//      {  1,  0,  0 },
//      { -1,  0,  0 },
//      {  0,  1,  0 },
//      {  0, -1,  0 },
//      {  0,  0,  1 },
//      {  0,  0, -1 },
//      {  1,  1,  0 },
//      { -1, -1,  0 },
//      {  1, -1,  0 },
//      { -1,  1,  0 },
//      {  1,  0,  1 },
//      { -1,  0, -1 },
//      {  1,  0, -1 },
//      { -1,  0,  1 },
//      {  0,  1,  1 },
//      {  0, -1, -1 },
//      {  0,  1, -1 },
//      {  0, -1,  1 },
//      {  1,  1,  1 },
//      { -1,  1,  1 },
//      {  1, -1,  1 },
//      { -1, -1,  1 },
//      {  1,  1, -1 },
//      { -1,  1, -1 },
//      {  1, -1, -1 },
//      { -1, -1, -1 }
//  };

// AMREX_GPU_DEVICE AMREX_FORCE_INLINE
// Real w[Q] =
// {
//     8./27.,
//     2./27.,
//     2./27.,
//     2./27.,
//     2./27.,
//     2./27.,
//     2./27.,
//     1./54.,
//     1./54.,
//     1./54.,
//     1./54.,
//     1./54.,
//     1./54.,
//     1./54.,
//     1./54.,
//     1./54.,
//     1./54.,
//     1./54.,
//     1./54.,
//     1./216.,
//     1./216.,
//     1./216.,
//     1./216.,
//     1./216.,
//     1./216.,
//     1./216.,
//     1./216.
// };

AMREX_GPU_DEVICE AMREX_FORCE_INLINE
    Real
    feqHeLuo(const Real &pressure, const RealVect &u, const int i)
{
    amrex::Real eu, uv, feqt;

    eu = e[i][0] * u[0] + e[i][1] * u[1] + e[i][2] * u[2];

    uv = (u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);

    feqt = w[i] * (pressure + p0 * (3.0 * eu + 4.5 * eu * eu - 1.5 * uv));

    return feqt;
}

AMREX_GPU_DEVICE AMREX_FORCE_INLINE
    Real
    feqQian(const Real &rhot, const RealVect &u, const int i)
{
    amrex::Real eu, uv, feqt;

    eu = e[i][0] * u[0] + e[i][1] * u[1] + e[i][2] * u[2];

    uv = (u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);

    feqt = w[i] * rhot * (1.0 + 3.0 * eu + 4.5 * eu * eu - 1.5 * uv);

    return feqt;
}

AMREX_GPU_DEVICE AMREX_FORCE_INLINE
    Real
    forceGuo(const RealVect &u, const RealVect &Ft, const int i)
{
    amrex::Real m, n, nf;

    m = ((e[i][0] - u[0]) * Ft[0] + (e[i][1] - u[1]) * Ft[1] + (e[i][2] - u[2]) * Ft[2]) / cs2;

    n = (e[i][0] * u[0] + e[i][1] * u[1] + e[i][2] * u[2]) / cs2;

    nf = (e[i][0] * Ft[0] + e[i][1] * Ft[1] + e[i][2] * Ft[2]) / cs2;

    return w[i] * (m + n * nf);
}

#endif
