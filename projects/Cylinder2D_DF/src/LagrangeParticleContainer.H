#ifndef LAGRANGEPARTICLECONTAINER_H_
#define LAGRANGEPARTICLECONTAINER_H_

#include <AMReX_MultiFab.H>
#include <AMReX_MultiFabUtil.H>
#include <AMReX_AmrParGDB.H>
#include <AMReX_Particles.H>
#include <AMReX_AmrParticles.H>

#include "D2Q9.H"

struct PIdx
{
    enum
    {
        fx = 0,
        fy,
        ufx,
        ufy,
        nattribs
    };
};

class MyParIter : public amrex::ParIter<0, 0, PIdx::nattribs>
{
public:
    using amrex::ParIter<0, 0, PIdx::nattribs>::ParIter;
    using RealVector = amrex::ParIter<0, 0, PIdx::nattribs>::ContainerType::RealVector;

    const std::array<RealVector, PIdx::nattribs>& GetAttribs() const
    {
        return GetStructOfArrays().GetRealData();
    }

    std::array<RealVector, PIdx::nattribs>& GetAttribs()
    {
        return GetStructOfArrays().GetRealData();
    }

    const RealVector& GetAttribs(int comp) const
    {
        return GetStructOfArrays().GetRealData(comp);
    }

    RealVector& GetAttribs(int comp)
    {
        return GetStructOfArrays().GetRealData(comp);
    }
};

//********************************************************************//
//                     继承AmrParticleContainer                       //
//********************************************************************//
// class LagrangeParticleContainer : public amrex::AmrParticleContainer<0, 0, PIdx::nattribs>
// {
// public:

//     LagrangeParticleContainer(amrex::AmrCore* amr_core)
//         :   amrex::AmrParticleContainer<0, 0, PIdx::nattribs>(amr_core)
//     {
//         amrex::Print()<<"Particle construct successed ..." << std:: endl;
//     }

//     ~LagrangeParticleContainer () override = default;

//     LagrangeParticleContainer ( const LagrangeParticleContainer &) = delete;
//     LagrangeParticleContainer& operator= ( const LagrangeParticleContainer & ) = delete;

//     LagrangeParticleContainer ( LagrangeParticleContainer && ) noexcept = default;
//     LagrangeParticleContainer& operator= ( LagrangeParticleContainer && ) noexcept = default;

//     void PrintParticleParm();

//     void InitParticle(int lev);

//     void MoveParticle();

//     void InterpForce(int lev, amrex::MultiFab& rho_lev, amrex::MultiFab& u_lev, amrex::MultiFab& force_lev);

//     void SaveFxy(int lev, int step);

//     void WriteParticle(int step);

// };


//********************************************************************//
//                     原版                                           //
//********************************************************************//
class LagrangeParticleContainer : public amrex::ParticleContainer<0, 0, PIdx::nattribs>
{
public:
    LagrangeParticleContainer(amrex::AmrCore* amr_core)
        :   amrex::ParticleContainer<0, 0, PIdx::nattribs>(amr_core->GetParGDB())
    {
        amrex::Print()<<"Particle construct successed ..." << std:: endl;
    }

    ~LagrangeParticleContainer () override = default;

    LagrangeParticleContainer ( const LagrangeParticleContainer &) = delete;
    LagrangeParticleContainer& operator= ( const LagrangeParticleContainer & ) = delete;

    LagrangeParticleContainer ( LagrangeParticleContainer && ) noexcept = default;
    LagrangeParticleContainer& operator= ( LagrangeParticleContainer && ) noexcept = default;

    void PrintParticleParm();

    void InitParticle(int lev);

    void MoveParticle();

    void InterpForce(int lev, amrex::MultiFab& rho_lev, amrex::MultiFab& u_lev, amrex::MultiFab& force_lev);

    void SaveFxy(int lev, int step);

    void WriteParticle(int step);

};

#endif //LAGRANGEPARTICLECONTAINER_H_