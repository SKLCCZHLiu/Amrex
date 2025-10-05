#ifndef D2Q9_H_
#define D2Q9_H_
#include <AMReX_Array4.H>

using namespace amrex;

#define DIM  2
#define Q    9

// const int NX = 128;
// const int NY = 128;
// const int NZ = 128;
// const Real Re = 1000;
// const Real dx = 1.0;
// const Real dt = dx;
// const Real c  = dx / dt;
// const Real cs = (c / sqrt(3.0));
// const Real cs2 = cs * cs;
// const Real rho0 = 1.0;
// const Real U0 = 0.1;
// const Real mv = U0 * NX * dx / Re;
// const Real tau = mv / (dt * cs * cs) + 0.5;
// const Real omega = 1 / tau;


#define NX    128
#define NY    128
#define NZ    128
#define LX    (NX - 1)
#define LY    (NY - 1)
#define LZ    (NZ - 1)
#define Re    1000
#define dx    (1.0)
#define dt    dx
#define c     1.0
#define cs    (c / sqrt(3.0))
#define cs2   (cs * cs)
#define rho0  1.0
#define U0    0.1
#define mv    (U0 * LX * dx / Re)
#define tau   (mv / (dt * cs2) + 0.5)
#define omega (1.0 / tau)

AMREX_GPU_DEVICE AMREX_FORCE_INLINE
IntVect e[Q] = 
{
    {  0,  0 },
    {  1,  0 },
    {  0,  1 },
    { -1,  0 },
    {  0, -1 },
    {  1,  1 },
    { -1,  1,},
    { -1, -1 },
    {  1, -1 }
};

AMREX_GPU_DEVICE AMREX_FORCE_INLINE
Real w[Q] = 
{
    4./9.,
    1./9.,
    1./9.,
    1./9.,
    1./9.,
    1./36.,
    1./36.,
    1./36.,
    1./36.
};

AMREX_GPU_DEVICE AMREX_FORCE_INLINE
Real feqQian(const Real& rho, const RealVect& u, const int i) 
{
    Real eu, uv, feq;

    eu  = e[i][0] * u[0] + e[i][1] * u[1];
    uv  = (u[0] * u[0] + u[1] * u[1]);
    feq = w[i] * rho * ( 1.0 + 3.0 * eu + 4.5 * eu * eu - 1.5 * uv);

    return feq;
}



#endif