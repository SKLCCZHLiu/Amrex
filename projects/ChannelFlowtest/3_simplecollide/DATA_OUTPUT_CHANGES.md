# Data Output Directory Changes

## Overview
All data output files are now written to the `data/` directory instead of the project root directory. This change improves project organization and makes it easier to manage output files.

## Modified Files

### 1. src/AmrCoreLBM.cpp
- **WriteMultiParticleFile()**: Particle data files (`particle_data*.dat`) now written to `data/`
- **RecordMeanVelocityProfile()**: Mean velocity profile (`mean_velocity_profile_final.txt`) now written to `data/`
- **SaveParticleDistance()**: Distance data (`dist.dat`) now written to `data/`

### 2. src/LagrangeParticleContainer.cpp
- **SaveVelocity()**: Velocity data files (`vel_*.dat`) now written to `data/`
- **SavePosition()**: Position data files (`pos_*.dat`) now written to `data/`
- **SaveFxy()**: Drag coefficient files (`CdCl_*.dat`) now written to `data/`

### 3. config/inputs
- **amr.plot_file**: Plot file prefix changed from `case_channelFlow_Ret180` to `data/case_channelFlow_Ret180`

## Output Files Now in data/ Directory

| File Pattern | Description | Function |
|-------------|-------------|----------|
| `particle_data*.dat` | Particle position and ID data | WriteMultiParticleFile() |
| `dist.dat` | Particle distance measurements | SaveParticleDistance() |
| `mean_velocity_profile_final.txt` | Final mean velocity profile | RecordMeanVelocityProfile() |
| `vel_*.dat` | Particle velocity data (one per particle) | SaveVelocity() |
| `pos_*.dat` | Particle position data (one per particle) | SavePosition() |
| `CdCl_*.dat` | Drag coefficient data (one per particle) | SaveFxy() |
| `case_channelFlow_Ret180*` | AMReX plot files | WriteMultiLevelPlotfile() |

## Notes

1. **Data Directory**: The `data/` directory must exist before running the simulation. It should already be present in the repository.

2. **Backward Compatibility**: Existing scripts or analysis tools that expect output files in the root directory will need to be updated to look in the `data/` directory instead.

3. **Clean Output**: This change makes it easier to clean up simulation output by simply removing or archiving the contents of the `data/` directory.

## Example Usage

After running a simulation, all output files will be located in:
```
projects/ChannelFlow/data/
├── particle_data000000.dat
├── particle_data000001.dat
├── dist.dat
├── mean_velocity_profile_final.txt
├── vel_0.dat
├── pos_0.dat
├── CdCl_0.dat
└── case_channelFlow_Ret180000000/
```

Instead of being scattered in the project root directory.
