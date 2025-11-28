#ifndef KERNELS_H_
#define KERNELS_H_

#include <AMReX_Box.H>
#include <AMReX_FArrayBox.H>
#include <AMReX_Geometry.H>
#include <AMReX_PhysBCFunct.H>

#include "D2Q9.H"

using namespace amrex;

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void init_fluid(int i, int j, int k, amrex::Array4<amrex::Real> const& fold,
                amrex::Array4<amrex::Real> const& fnew) {
    for (int q = 0; q < Q; q++) {
        fold(i, j, k, q) = feqHeLuo(p0, {U0, 0.0}, q);
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
    }

    u(i, j, k, 0) = uxt / p0;
    u(i, j, k, 1) = uyt / p0;
    rho(i, j, k, 0) = rhot;
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void compute_vorticity(int i, int j, int k, amrex::Array4<amrex::Real> const& u,
                       amrex::Array4<amrex::Real> const& vort, amrex::Real dt) {
    Real u_x = (u(i + 1, j, k, 0) - u(i - 1, j, k, 0)) / (2 * dt);
    Real u_y = (u(i, j + 1, k, 0) - u(i, j - 1, k, 0)) / (2 * dt);
    Real u_z = 0.0;
    Real v_x = (u(i + 1, j, k, 1) - u(i - 1, j, k, 1)) / (2 * dt);
    Real v_y = (u(i, j + 1, k, 1) - u(i, j - 1, k, 1)) / (2 * dt);
    Real v_z = 0.0;
    Real w_x = 0.0;
    Real w_y = 0.0;
    Real w_z = 0.0;

    Real WX = w_y - v_z;
    Real WY = u_z - w_x;
    Real WZ = v_x - u_y;

    vort(i, j, k, 0) = sqrt(WX * WX + WY * WY + WZ * WZ); // 不知道这样直接从三维改到二维对不对
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void compute_shear(int i, int j, int k, amrex::Array4<amrex::Real> const& u,
                   amrex::Array4<amrex::Real> const& shear, amrex::Real dt) {
    Real SS, Q_CSM, E_CSM, F_CSM, C_CSM;

    Real u_x = (u(i + 1, j, k, 0) - u(i - 1, j, k, 0)) / (2 * dt);
    Real u_y = (u(i, j + 1, k, 0) - u(i, j - 1, k, 0)) / (2 * dt);
    Real u_z = 0.0;
    Real v_x = (u(i + 1, j, k, 1) - u(i - 1, j, k, 1)) / (2 * dt);
    Real v_y = (u(i, j + 1, k, 1) - u(i, j - 1, k, 1)) / (2 * dt);
    Real v_z = 0.0;
    Real w_x = 0.0;
    Real w_y = 0.0;
    Real w_z = 0.0;

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
        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;

        for (int q = 0; q < Q; q++) {
            ft1 = fold(x1, y1, z1, q);
            rhot1 += ft1;
            uxt1 += ft1 * e[q][0];
            uyt1 += ft1 * e[q][1];
        }

        uxt1 /= p0;
        uyt1 /= p0;

        // uxt = 0.0;
        // uyt = 0.0;

        uxt = U0;
        uyt = 0.0;

        rhot = rhot1;

        for (int q = 0; q < Q; q++) {
            fnew(x, y, z, q) = feqHeLuo(rhot, {uxt, uyt}, q) + fold(x1, y1, z1, q) - feqHeLuo(rhot1, {uxt1, uyt1}, q);
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
        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;

        for (int q = 0; q < Q; q++) {
            ft1 = fold(x1, y1, z1, q);
            rhot1 += ft1;
            uxt1 += ft1 * e[q][0];
            uyt1 += ft1 * e[q][1];
        }

        uxt1 /= p0;
        uyt1 /= p0;

        // uxt = 0.0;
        // uyt = 0.0;

        uxt = uxt1;
        uyt = uyt1;

        rhot = rhot1;

        for (int q = 0; q < Q; q++) {
            fnew(x, y, z, q) = feqHeLuo(rhot, {uxt, uyt}, q) + fold(x1, y1, z1, q) - feqHeLuo(rhot1, {uxt1, uyt1}, q);
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
        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;

        for (int q = 0; q < Q; q++) {
            ft1 = fold(x1, y1, z1, q);
            rhot1 += ft1;
            uxt1 += ft1 * e[q][0];
            uyt1 += ft1 * e[q][1];
        }

        uxt1 /= p0;
        uyt1 /= p0;

        // uxt = 0.0;
        // uyt = 0.0;

        uxt = uxt1;
        uyt = uyt1;

        rhot = rhot1;

        for (int q = 0; q < Q; q++) {
            fnew(x, y, z, q) = feqHeLuo(rhot, {uxt, uyt}, q) + fold(x1, y1, z1, q) - feqHeLuo(rhot1, {uxt1, uyt1}, q);
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
        Real uxt = 0.0, uyt = 0.0, uzt = 0.0, rhot = 0.0, ft = 0.0;

        for (int q = 0; q < Q; q++) {
            ft1 = fold(x1, y1, z1, q);
            rhot1 += ft1;
            uxt1 += ft1 * e[q][0];
            uyt1 += ft1 * e[q][1];
        }

        uxt1 /= p0;
        uyt1 /= p0;

        // uxt = U0;
        // uyt = 0.0;

        uxt = uxt1;
        uyt = uyt1;

        rhot = rhot1;

        for (int q = 0; q < Q; q++) {
            fnew(x, y, z, q) = feqHeLuo(rhot, {uxt, uyt}, q) + fold(x1, y1, z1, q) - feqHeLuo(rhot1, {uxt1, uyt1}, q);
        }
    }

    if (i < 0 || i > hi[0] || j < 0 || j > hi[1]) {
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
    }

    Ftx = Ft(i, j, k, 0);
    Fty = Ft(i, j, k, 1);

    uxt /= p0;
    uyt /= p0;

    uxt += 0.5 * dt * Ftx / (p0 / (cs * cs));
    uyt += 0.5 * dt * Fty / (p0 / (cs * cs));

    // if((i >= 1) && (i < hi[0]) && (j >= 1) && (j < hi[1]))
    // {
    for (int q = 0; q < Q; q++) {
        Real forceEx = (1 - 0.5 * omega) * dt * forceGuo({uxt, uyt}, {Ftx, Fty}, q) * cs2;
        fold(i, j, k, q) = (1 - omega) * fold(i, j, k, q) + omega * feqHeLuo(rhot, {uxt, uyt}, q) + forceEx;
    }
    // }
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void stream(int i, int j, int k, amrex::Array4<amrex::Real> const& fold, amrex::Array4<amrex::Real> const& fnew,
            amrex::IntVect const& hi, const bool is_finest) {
    for (int q = 0; q < Q; q++) {
        int xm = i - e[q][0];
        int ym = j - e[q][1];
        int zm = 0;

        // if(is_finest)
        // {
        //     if((i >= 1) && (i < hi[0]) && (j >= 1) && (j < hi[1]))
        //     {
        //         fnew(i, j, k, q) = fold(xm, ym, zm, q);
        //     }
        // }
        // else
        // {
        //     fnew(i, j, k, q) = fold(xm, ym, zm, q);
        // }
        if ((i >= 1) && (i < hi[0]) && (j >= 1) && (j < hi[1])) {
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
    Real uxt = 0.0, uyt = 0.0, rhot = 0.0, ft = 0.0, res = 0.0;

    for (int q = 0; q < Q; q++) {
        ft = fold(i, j, k, q);
        rhot += ft;
        uxt += ft * e[q][0];
        uyt += ft * e[q][1];
    }

    uxt = uxt / p0;
    uyt = uyt / p0;

    for (int q = 0; q < Q; q++) {
        res = feqHeLuo(rhot, {uxt, uyt}, q);
        fnew(i, j, k, q) = res + (fold(i, j, k, q) - res) * scale;
    }
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void average_scale(int i, int j, int k, amrex::Array4<amrex::Real> const& fold, amrex::Real scale) {

    Real uxt = 0.0, uyt = 0.0, rhot = 0.0, ft = 0.0, res = 0.0;

    for (int q = 0; q < Q; q++) {
        ft = fold(i, j, k, q);
        rhot += ft;
        uxt += ft * e[q][0];
        uyt += ft * e[q][1];
    }

    uxt = uxt / p0;
    uyt = uyt / p0;

    for (int q = 0; q < Q; q++) {
        res = feqHeLuo(rhot, {uxt, uyt}, q);
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

AMREX_GPU_HOST_DEVICE
AMREX_FORCE_INLINE
void state_error(int i, int j, int k,
                 amrex::Array4<char> const& tag,
                 amrex::Array4<amrex::Real const> const& vort,
                 amrex::Real phierr, char tagval, char clearval, int lev, amrex::Real dx,
                 amrex::IntVect const& lo2, amrex::IntVect const& hi2) {
    Real x_r = X * dx_0 / dx;
    Real y_r = Y * dx_0 / dx;
    Real r_r = R * dx_0 / dx;

    Real dist = sqrt((i - x_r) * (i - x_r) + (j - y_r) * (j - y_r));

    // 根据固体表面距离加密
    if (dist <= r_r + 10) {
        tag(i, j, k) = tagval;
    }

    // 网格自适应加密
    if (lev == 0) {
        if (vort(i, j, k, 0) >= 0.1) {
            tag(i, j, k) = tagval;
        }
    } else if (lev == 1) {
        if (vort(i, j, k, 0) >= 0.4) {
            tag(i, j, k) = tagval;
        }
    } else if (lev == 2) {
        if (vort(i, j, k, 0) >= 0.6 && vort(i, j, k, 0) <= 4.0 && j >= lo2[1] && j <= hi2[1]) {
            tag(i, j, k) = tagval;
        }
    }

    // //网格自适应加密
    // if(lev == 0)
    // {
    //     if(vort(i, j, k, 0) >= 0.4)
    //     {
    //         tag(i,j,k) = tagval;
    //     }
    // }
    // else if(lev == 1)
    // {
    //     if(vort(i, j, k, 0) >= 0.6 && vort(i, j, k, 0) <= 4.0)
    //     {
    //         tag(i,j,k) = tagval;
    //     }
    // }

    // //网格自适应加密
    // if(lev == 0)
    // {
    //     if(vort(i, j, k, 0) >= 0.6 && vort(i, j, k, 0) <= 4.0)
    //     {
    //         tag(i,j,k) = tagval;
    //     }
    // }

    // //触碰边界处加密清除
    // if(i >= (nx-0.1)/dx || i <= 0.1/dx || j >= (ny-0.1)/dx || j <= 0.1/dx)
    // {
    //     tag(i,j,k) = clearval;
    // }
}

AMREX_GPU_HOST_DEVICE
AMREX_FORCE_INLINE
void state_error_2(int i, int j, int k,
                   amrex::Array4<char> const& tag,
                   amrex::Array4<amrex::Real const> const& vort,
                   amrex::Real phierr, char tagval, char clearval, int lev, amrex::Real dx,
                   amrex::IntVect const& lo2, amrex::IntVect const& hi2) {
    Real x_r = X * dx_0 / dx;
    Real y_r = Y * dx_0 / dx;
    Real r_r = R * dx_0 / dx;

    Real dist = sqrt((i - x_r) * (i - x_r) + (j - y_r) * (j - y_r));

    // 根据固体表面距离加密
    if (/*dist >= r_r-20 &&*/ dist <= r_r + 30) {
        tag(i, j, k) = tagval;
    }

    // 网格自适应加密
    if (lev == 0) {
        if (vort(i, j, k, 0) >= phierr && vort(i, j, k, 0) < 0.8 &&
            i >= lo2[0] && i <= hi2[0] && j >= lo2[1] && j <= hi2[1]) {
            tag(i, j, k) = tagval;
        }
    } else if (lev == 1) {
        if (vort(i, j, k, 0) >= phierr && vort(i, j, k, 0) < 1.0 &&
            i >= lo2[0] && i <= hi2[0] && j >= lo2[1] && j <= hi2[1]) // 自适应
        {
            tag(i, j, k) = tagval;
        }
    } else if (lev == 2) {
        if (vort(i, j, k, 0) >= phierr && vort(i, j, k, 0) < 1.2 &&
            i >= lo2[0] && i <= hi2[0] && j >= lo2[1] && j <= hi2[1]) // 自适应
        {
            tag(i, j, k) = tagval;
        }
    }

    // 触碰边界处加密清除
    if (i >= (nx - 0.1) / dx || i <= 0.1 / dx || j >= (ny - 0.1) / dx || j <= 0.1 / dx) {
        tag(i, j, k) = clearval;
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
    force_interp_extrap(P const& p, amrex::Real& fx, amrex::Real& fy, /*amrex::Real& fz,*/ amrex::Real& ufx, amrex::Real& ufy,
                        /*amrex::Real& ufz, amrex::Real& area,*/ amrex::Array4<amrex::Real> const& u,
                        amrex::Array4<amrex::Real> const& rho, amrex::Array4<amrex::Real> const& Ft, amrex::Real const delta) {
    amrex::Real xt, yt, zt, lx, ly, lz;
    amrex::Real IB_Interp;
    amrex::Real uxt, uyt, uzt, rhot, upx, upy, upz, fxt, fyt, fzt;

    xt = p.pos(0) / delta;
    yt = p.pos(1) / delta;

    uxt = 0.0;
    uyt = 0.0;
    uzt = 0.0;
    rhot = 0.0;
    upx = 0.0;
    upy = 0.0;
    upz = 0.0;

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            int xx = static_cast<int>(amrex::Math::floor(xt)) + x; // 拉格朗日点作用范围内真实的点坐标
            int yy = static_cast<int>(amrex::Math::floor(yt)) + y;
            int zz = 0;

            lx = xt - (xx + 0.5);
            ly = yt - (yy + 0.5);

            IB_Interp = delta3p(lx) * delta3p(ly);

            uxt += u(xx, yy, zz, 0) * IB_Interp;
            uyt += u(xx, yy, zz, 1) * IB_Interp;
            rhot += rho(xx, yy, zz, 0) * IB_Interp;
        }
    }

    rhot = 2 * rhot / cs2 / dt_min;

    fxt = rhot * (uxt - upx);
    fyt = rhot * (uyt - upy);

    fx = fxt * IB_weight * dx_min * dx_min;
    fy = fyt * IB_weight * dx_min * dx_min;

    ufx = uxt;
    ufy = uyt;

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            int xx = static_cast<int>(amrex::Math::floor(xt)) + x;
            int yy = static_cast<int>(amrex::Math::floor(yt)) + y;
            int zz = 0;

            lx = xt - (xx + 0.5);
            ly = yt - (yy + 0.5);

            IB_Interp = delta3p(lx) * delta3p(ly) * IB_weight;

            amrex::Gpu::Atomic::AddNoRet(&Ft(xx, yy, zz, 0), -fxt * IB_Interp);
            amrex::Gpu::Atomic::AddNoRet(&Ft(xx, yy, zz, 1), -fyt * IB_Interp);
        }
    }
}

#endif