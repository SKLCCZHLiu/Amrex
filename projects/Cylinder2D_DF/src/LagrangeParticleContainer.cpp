#include "LagrangeParticleContainer.H"
#include "D2Q9.H"
#include "Kernels.H"
using namespace amrex;


void LagrangeParticleContainer::PrintParticleParm()
{
    amrex::Print() << "╔══════════════════════════════════════════════════════╗" << std::endl;
    amrex::Print() << "║                      IBM PARAMETERS                  ║" << std::endl;
    amrex::Print() << "╚══════════════════════════════════════════════════════╝" << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  R      =" << std::setw(10) << std::right << R << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  Rm     =" << std::setw(10) << std::right << Rm << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  ds0    =" << std::setw(10) << std::right << ds0 << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  np     =" << std::setw(10) << std::right << np << std::endl;
    amrex::Print() << std::setw(15) << std::left << "  area   =" << std::setw(10) << std::right << IB_weight << std::endl;

    amrex::Print() << "╚══════════════════════════════════════════════════════╝" << std::endl;
    amrex::Print() << std::endl;
}

void LagrangeParticleContainer::InitParticle(int lev)
{
    ParticleType p;
    std::array<ParticleReal, PIdx::nattribs> attribs;
    const Real* delta = Geom(0).CellSize();

    int num_proc = ParallelDescriptor::NProcs();

    if(ParallelDescriptor::MyProc() == ParallelDescriptor::IOProcessorNumber()) 
    {
        for(int i = 0; i < np; i++)
        {
            p.id()  = ParticleType::NextID();
            p.cpu() = ParallelDescriptor::MyProc(); //int(p.id()%num_proc);
            p.pos(0) = (X + Rm * cos(2.0 * PI * (i) / np)) * delta[0];
            p.pos(1) = (Y + Rm * sin(2.0 * PI * (i) / np)) * delta[0];
          
            attribs[PIdx::fx]  = 0.0;
            attribs[PIdx::fy]  = 0.0;
            attribs[PIdx::ufx] = 0.0;
            attribs[PIdx::ufy] = 0.0;

            std::pair<int, int> key {0, 0};
            auto& particle_tile = GetParticles(0)[key];
            
            particle_tile.push_back(p);
            particle_tile.push_back_real(attribs);    
        }
    }
    Redistribute();
}

void LagrangeParticleContainer::MoveParticle()
{

}

void LagrangeParticleContainer::InterpForce(int lev, amrex::MultiFab& rho_lev, amrex::MultiFab& u_lev, amrex::MultiFab& force_lev)
{
    const Real delta = Geom(lev).CellSize()[0];

    for(MyParIter pti(*this, lev); pti.isValid(); ++pti)
    {
        auto& particles = pti.GetArrayOfStructs();
        auto  p_ptr     = particles().data();
        const long  n  = pti.numParticles();

        auto& attribs = pti.GetAttribs();
        auto  fx      = attribs[PIdx::fx].data();
        auto  fy      = attribs[PIdx::fy].data();
        auto  ufx     = attribs[PIdx::ufx].data();
        auto  ufy     = attribs[PIdx::ufy].data();

        const Array4<Real>& u   = u_lev.array(pti);
        const Array4<Real>& rho = rho_lev.array(pti);
        const Array4<Real>& Ft  = force_lev.array(pti);        

        amrex::ParallelFor(n, [=]AMREX_GPU_DEVICE(int i) noexcept
        {
            force_interp_extrap(p_ptr[i], fx[i], fy[i], ufx[i], ufy[i], u, rho, Ft, delta);
        });
    }
}

void LagrangeParticleContainer::SaveFxy(int lev, int step)
{
    using SPType  = typename LagrangeParticleContainer::SuperParticleType;

    auto fx = amrex::ReduceSum(*this, [=]AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal
    {
        return p.rdata(PIdx::fx);
    });

    auto fy = amrex::ReduceSum(*this, [=]AMREX_GPU_HOST_DEVICE(const SPType& p) -> ParticleReal
    {
        return p.rdata(PIdx::fy);
    });

    ParallelDescriptor::ReduceRealSum(fx);
    ParallelDescriptor::ReduceRealSum(fy);

    if(ParallelDescriptor::MyProc() == ParallelDescriptor::IOProcessorNumber())
    {
        FILE* file;

        Real m = 0.5 * (p0 / cs2) * U0 * U0 * D * dx_0;

        fx /= m;
        fy /= m;

        if((file = fopen("CdCl.dat", "a")) == NULL)
        {
            printf("can not open the file!");
            exit(0);
        }
        fprintf(file, "%d\t%f\t%f\n", step, fx, fy);
        fclose(file);	
    }
}

void LagrangeParticleContainer::WriteParticle(int step)
{
    const std::string& pltfile = amrex::Concatenate("particles", step, 5);
    WriteAsciiFile(pltfile);      
}
