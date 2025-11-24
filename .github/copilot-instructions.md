# LBM-AMR Simulation Project - AI Agent Instructions

## Project Architecture Overview

This is a high-performance computational fluid dynamics simulation using **Lattice Boltzmann Method (LBM)** with **Adaptive Mesh Refinement (AMR)** based on the AMReX framework. The project simulates complex fluid flows around static/moving spheres using immersed boundary methods.

### Core Components
- **main.cpp**: Program entry point, AMReX initialization, time loop management
- **AmrCoreLBM.{H,cpp}**: AMR framework implementation, manages MultiFab data structures
- **Kernels.H**: GPU-optimized computational kernels (collision, streaming, boundary conditions)
- **D3Q19.H**: LBM model parameters (D3Q19 lattice, weights, relaxation times)
- **LagrangeParticleContainer.{H,cpp}**: Lagrangian particle management for immersed boundaries
- **inputs**: Runtime configuration file (AMR levels, time steps, output intervals)

### Key Design Patterns

#### 1. GPU Kernel Organization
All computational kernels in `Kernels.H` must have `AMREX_GPU_DEVICE` and `AMREX_FORCE_INLINE` decorators:
```cpp
AMREX_GPU_DEVICE AMREX_FORCE_INLINE
void compute_macro(int i, int j, int k, amrex::Array4<amrex::Real> const &fold,
                   amrex::Array4<amrex::Real> const &rho, amrex::Array4<amrex::Real> const &u)
```

#### 2. Multi-Level AMR Data Management
Use `MultiFab` arrays for multi-level data:
```cpp
amrex::Vector<amrex::MultiFab> f_old;  // Old distribution functions
amrex::Vector<amrex::MultiFab> f_new;  // New distribution functions
amrex::Vector<amrex::MultiFab> rho;    // Density fields
amrex::Vector<amrex::MultiFab> u;      // Velocity fields
```

#### 3. LBM Collision Models
Support multiple collision operators:
- **BGK**: `collide_bgk()` - Single relaxation time
- **Cumulant**: `collide_cumulant()` - Multiple relaxation times for better stability
- **Regularized**: `collide_regularized()` - Improved Galilean invariance

#### 4. Boundary Conditions
- **Wall boundaries**: `fill_boundary()` with bounce-back scheme
- **Periodic boundaries**: AMReX built-in periodic BC
- **Inlet/Outlet**: Density-driven pressure boundaries

## Build System & Dependencies

### Compilation Requirements
- **AMReX 23.09+**: Set `AMREX_HOME` environment variable
- **CUDA 11.7+**: For GPU acceleration
- **MPI**: For parallel execution
- **GCC 10.3+**: Compiler toolchain

### Build Configuration (GNUmakefile)
```makefile
AMREX_HOME ?= /path/to/amrex-23.09/
DEBUG = FALSE
TINY_PROFILE = TRUE  # Required for performance profiling
DIM = 3
USE_MPI = TRUE
USE_PARTICLES = TRUE
USE_CUDA = TRUE
```

### Build Commands
```bash
# Compile the project
make -f GNUmakefile -j8

# Clean build artifacts
make -f GNUmakefile clean

# Rebuild from scratch
make -f GNUmakefile realclean
make -f GNUmakefile -j8
```

## Runtime Configuration

### Key Parameters (inputs file)
```ini
# Time stepping
max_step = 80000
stop_time = 2500000.0

# AMR configuration
amr.n_cell = 128 64 64      # Base grid resolution
amr.max_level = 2            # Maximum refinement levels
amr.ref_ratio = 2 2 2 2      # Refinement ratios

# LBM parameters
lbm.tau = 0.6                # Relaxation time
lbm.err = 0.6 0.8 1.0 1.2    # Refinement criteria

# Output control
amr.plot_int = 200           # Plot interval
amr.plot_file = case_name_   # Output file prefix
```

### Execution Commands
```bash
# Run with MPI + CUDA
mpirun -n 4 ./main3d.gnu.TPROF.MPI.CUDA.ex inputs

# Run on specific GPUs
mpirun -n 4 --mca btl_openib_warn_default_gid_prefix 0 ./main3d.gnu.TPROF.MPI.CUDA.ex inputs
```

## Development Workflow

### Adding New LBM Kernels
1. Define kernel function in `Kernels.H` with proper decorators
2. Implement collision/streaming logic following LBM patterns
3. Add kernel call in `AmrCoreLBM.cpp` using `ParallelFor`
4. Test on both CPU and GPU configurations

### Modifying Boundary Conditions
1. Update `fill_boundary()` function in `Kernels.H`
2. Handle 6 boundary faces (i=0, i=hi[0], j=0, j=hi[1], k=0, k=hi[2])
3. Implement appropriate bounce-back or pressure boundary schemes

### AMR Refinement Criteria
1. Define error estimation functions in `Kernels.H`
2. Use velocity gradients, density variations, or vorticity
3. Update `inputs` file with appropriate `lbm.err` values

## Common Patterns & Conventions

### Memory Management
- Use AMReX `MultiFab` for distributed arrays
- Always check `isValid()` before accessing MFIter
- Use `growntilebox()` for ghost cell access

### GPU Optimization
- Prefer `ParallelFor` over manual kernel launches
- Use `AMREX_GPU_DEVICE` functions for device code
- Minimize host-device memory transfers

### Error Handling
- Check CUDA errors with `AMREX_GPU_ERROR_CHECK()`
- Use AMReX assertion macros for debugging
- Monitor GPU memory usage in long simulations

### File Organization
- Header files (.H) contain class definitions and inline functions
- Implementation files (.cpp) contain method implementations
- Configuration files (inputs, GNUmakefile) in project root
- Output data in case-specific subdirectories

## Integration Points

### External Dependencies
- **AMReX**: Core framework for AMR and parallel computing
- **CUDA**: GPU acceleration library
- **MPI**: Inter-process communication
- **HDF5**: Data output (optional)

### Cross-Component Communication
- `main.cpp` ↔ `AmrCoreLBM` ↔ `Kernels.H`: Main computation pipeline
- `AmrCoreLBM` ↔ `LagrangeParticleContainer`: Fluid-particle coupling
- `Kernels.H` ↔ `D3Q19.H`: LBM model parameters
- All components share `inputs` configuration

## Performance Considerations

### GPU Memory Layout
- Use AMReX `Array4` for efficient GPU memory access
- Prefer structure-of-arrays over array-of-structures
- Align data access patterns with GPU warp size

### Load Balancing
- AMReX handles automatic domain decomposition
- Monitor load imbalance with `amr.grid_eff` parameter
- Adjust `max_grid_size` for optimal GPU utilization

### Profiling
- Use `TINY_PROFILE=TRUE` for basic timing
- Enable `DEBUG=TRUE` for detailed diagnostics
- Monitor GPU utilization with `nvidia-smi`

## Key Files for Reference
- `Kernels.H`: Core computational algorithms
- `AmrCoreLBM.cpp`: AMR framework integration
- `D3Q19.H`: LBM model constants
- `inputs`: Configuration examples
- `GNUmakefile`: Build system template
