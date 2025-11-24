#ifndef LAGRANGEPARTICLECONTAINER_H_
#define LAGRANGEPARTICLECONTAINER_H_

#include <AMReX_MultiFab.H>
#include <AMReX_MultiFabUtil.H>
#include <AMReX_AmrParGDB.H>
#include <AMReX_Particles.H>
#include <AMReX_AmrParticles.H>

#include "D3Q19.H"

struct PIdx
{
    enum
    {
        fx = 0,
        fy,
        fz,
        tx,
        ty,
        tz,
        xlocal,
        ylocal,
        zlocal,
        area,
        nattribs
    };
};

class MyParIter : public amrex::ParIter<0, 0, PIdx::nattribs>
{
public:
    using amrex::ParIter<0, 0, PIdx::nattribs>::ParIter;
    using RealVector = amrex::ParIter<0, 0, PIdx::nattribs>::ContainerType::RealVector;

    const std::array<RealVector, PIdx::nattribs> &GetAttribs() const
    {
        return GetStructOfArrays().GetRealData();
    }

    std::array<RealVector, PIdx::nattribs> &GetAttribs()
    {
        return GetStructOfArrays().GetRealData();
    }

    const RealVector &GetAttribs(int comp) const
    {
        return GetStructOfArrays().GetRealData(comp);
    }

    RealVector &GetAttribs(int comp)
    {
        return GetStructOfArrays().GetRealData(comp);
    }
};

//********************************************************************//
//                     原版                                           //
//********************************************************************//
class LagrangeParticleContainer : public amrex::ParticleContainer<0, 0, PIdx::nattribs>
{
public:
    LagrangeParticleContainer(amrex::AmrCore *amr_core)
        : amrex::ParticleContainer<0, 0, PIdx::nattribs>(amr_core->GetParGDB())
    {
        radius = rate * R; // lev=0网格下的解析度, 算球体可能要内缩一下

        centre = {AMREX_D_DECL(X, Y, Z)}; // lev=0网格下

        F_tot = {AMREX_D_DECL(0., 0., 0.)};
        T_tot = {AMREX_D_DECL(0., 0., 0.)};
        vel = {AMREX_D_DECL(0., 0., 0.)};
        vel_old = {AMREX_D_DECL(0., 0., 0.)};
        angvel = {AMREX_D_DECL(0., 0., 0.)};
        angvel_old = {AMREX_D_DECL(0., 0., 0.)};

        amrex::Print() << "Particle construct successed ..." << std::endl;
    }

    LagrangeParticleContainer(amrex::AmrCore *amr_core, amrex::RealVect &point, const int idx)
        : amrex::ParticleContainer<0, 0, PIdx::nattribs>(amr_core->GetParGDB())
    {
        radius = rb; // lev=0网格下的解析度, 算球体可能要内缩一下
        id = idx;
        centre = {AMREX_D_DECL(point[0], point[1], point[2])}; // lev=0网格下

        F_tot = {AMREX_D_DECL(0., 0., 0.)};
        T_tot = {AMREX_D_DECL(0., 0., 0.)};
        F_lub = {AMREX_D_DECL(0., 0., 0.)};
        vel = {AMREX_D_DECL(0., 0., 0.)};
        vel_old = {AMREX_D_DECL(0., 0., 0.)};
        angvel = {AMREX_D_DECL(0., 0., 0.)};
        angvel_old = {AMREX_D_DECL(0., 0., 0.)};

        amrex::Print() << "Particle " << idx << " construct successed ..." << std::endl;
    }

    ~LagrangeParticleContainer() override = default;

    LagrangeParticleContainer(const LagrangeParticleContainer &) = delete;
    LagrangeParticleContainer &operator=(const LagrangeParticleContainer &) = delete;

    LagrangeParticleContainer(LagrangeParticleContainer &&) noexcept = default;
    LagrangeParticleContainer &operator=(LagrangeParticleContainer &&) noexcept = default;

    void PrintParticleParm();

    void InitParticle(int lev);
    void InitParticleFromFile(int lev, const std::string object);

    void MoveParticle(int lev, amrex::Real cur_time);

    amrex::RealVect ReturnCentre();
    amrex::RealVect ReturnVelocity();

    void SaveVelocity(int lev, int step);
    void SetLubVal(amrex::RealVect &force_lub);
    void SavePosition(int lev, int step);

    void InterpForce(int lev, amrex::MultiFab &rho_lev, amrex::MultiFab &u_lev, amrex::MultiFab &force_lev);
    void InterpForceWallModel(int lev, amrex::MultiFab &rho_lev, amrex::MultiFab &u_lev, amrex::MultiFab &force_lev);

    void SaveFxy(int lev, int step);

    void WriteParticle(int step);

    void CollideParticle(const std::unique_ptr<LagrangeParticleContainer> &p2);
    void CollideWall();

private: // 颗粒信息
    int id;

    amrex::Real radius;

    amrex::RealVect centre;

    amrex::RealVect F_tot;
    amrex::RealVect T_tot;
    amrex::RealVect F_lub;

    amrex::RealVect vel;
    amrex::RealVect vel_old;
    amrex::RealVect angvel;
    amrex::RealVect angvel_old;
};

#endif // LAGRANGEPARTICLECONTAINER_H_