#ifndef AUXILIARYPOINTCONTAINER_H_
#define AUXILIARYPOINTCONTAINER_H_

#include <AMReX_MultiFab.H>
#include <AMReX_MultiFabUtil.H>
#include <AMReX_AmrParGDB.H>
#include <AMReX_Particles.H>
#include <AMReX_AmrParticles.H>

#include "D3Q19.H"

struct PIdx2 {
    enum {
        cp = 0,
        nattribs
    };
};

class MyParIter2 : public amrex::ParIter<0, 0, PIdx2::nattribs> {
  public:
    using amrex::ParIter<0, 0, PIdx2::nattribs>::ParIter;
    using RealVector = amrex::ParIter<0, 0, PIdx2::nattribs>::ContainerType::RealVector;

    const std::array<RealVector, PIdx2::nattribs>& GetAttribs() const {
        return GetStructOfArrays().GetRealData();
    }

    std::array<RealVector, PIdx2::nattribs>& GetAttribs() {
        return GetStructOfArrays().GetRealData();
    }

    const RealVector& GetAttribs(int comp) const {
        return GetStructOfArrays().GetRealData(comp);
    }

    RealVector& GetAttribs(int comp) {
        return GetStructOfArrays().GetRealData(comp);
    }
};

class AuxiliaryPointContainer : public amrex::ParticleContainer<0, 0, PIdx2::nattribs> {
  public:
    AuxiliaryPointContainer(amrex::AmrCore* amr_core, amrex::RealVect& point, const int idx)
        : amrex::ParticleContainer<0, 0, PIdx2::nattribs>(amr_core->GetParGDB()) {
        radius = rb; // lev=0网格下的解析度
        id = idx;
        centre = {AMREX_D_DECL(point[0], point[1], point[2])}; // lev=0网格下

        amrex::Print() << "Auxiliary point " << idx << " construct successed ..." << std::endl;
    }

    ~AuxiliaryPointContainer() override = default;

    AuxiliaryPointContainer(const AuxiliaryPointContainer&) = delete;
    AuxiliaryPointContainer& operator=(const AuxiliaryPointContainer&) = delete;

    AuxiliaryPointContainer(AuxiliaryPointContainer&&) noexcept = default;
    AuxiliaryPointContainer& operator=(AuxiliaryPointContainer&&) noexcept = default;

    void InitCpPoint(int lev);

    void InterpCp(int lev, amrex::MultiFab& rho_lev);

    void WriteCp(int step);

  private: // 颗粒信息
    int id;

    amrex::Real radius;
    amrex::RealVect centre;
};

#endif