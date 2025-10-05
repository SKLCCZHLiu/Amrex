#include "AuxiliaryPointContainer.H"
#include "D3Q19.H"
#include "Kernels.H"
using namespace amrex;

//初始化表面的点
void AuxiliaryPointContainer::InitCpPoint(int lev)
{
    ParticleType p;
    std::array<ParticleReal, PIdx2::nattribs> attribs;
    const Real* delta = Geom(0).CellSize();

    int nump = 181;

    Real rt = radius + 0.0;
    Real alpha = 2 * PI / (nump-1);

    if(ParallelDescriptor::MyProc() == ParallelDescriptor::IOProcessorNumber()) 
    {
        for(int j = 0; j < nump; j++)
        {
            p.id()  = j; //ParticleType::NextID();
            p.cpu() = ParallelDescriptor::MyProc();

            p.pos(0) = (centre[0]) * delta[0] - rt * cos(j * alpha) * dx_min;
            p.pos(1) = (centre[1]) * delta[0];
            p.pos(2) = (centre[2]) * delta[0] + rt * sin(j * alpha) * dx_min;                         

            attribs[PIdx2::cp] = 0.0;

            std::pair<int, int> key {0, 0};
            auto& particle_tile = GetParticles(0)[key];
        
            particle_tile.push_back(p);
            particle_tile.push_back_real(attribs);    
        }
    }
    Redistribute();
}


void AuxiliaryPointContainer::InterpCp(int lev, amrex::MultiFab& rho_lev)
{
    const Real delta = Geom(lev).CellSize()[0];

    for(MyParIter2 pti(*this, lev); pti.isValid(); ++pti)
    {
        auto& particles = pti.GetArrayOfStructs();
        auto  p_ptr     = particles().data();
        const long  n   = pti.numParticles();

        auto& attribs = pti.GetAttribs();
        auto  cp      = attribs[PIdx2::cp].data();

        const Array4<Real>& rho = rho_lev.array(pti);

        amrex::ParallelFor(n, [=]AMREX_GPU_DEVICE(int i) noexcept
        {
            pressure_interp(p_ptr[i], cp[i], rho, delta);
        });
    }
}

void AuxiliaryPointContainer::WriteCp(int step)
{
    std::string filename = "cp_" + std::to_string(id);
    filename += '_';
    const std::string& pltfile = amrex::Concatenate(filename, step, 6);
    WriteAsciiFile(pltfile);  
}
