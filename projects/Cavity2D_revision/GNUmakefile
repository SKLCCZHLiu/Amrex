AMREX_HOME ?=/home/huazkjdxmrsgjzdsyshi/whcs-share18/wangyan/amrex-23.09/

DEBUG	= FALSE

TINY_PROFILE = FALSE

DIM	= 2

COMP    = gcc
BL_NO_FORT = TRUE
USE_MPI   = TRUE
USE_PARTICLES = TRUE
USE_OMP   = FALSE
USE_CUDA  = TRUE
USE_HIP   = FALSE

include $(AMREX_HOME)/Tools/GNUMake/Make.defs

include ./Make.package
include $(AMREX_HOME)/Src/Base/Make.package
include $(AMREX_HOME)/Src/AmrCore/Make.package
include $(AMREX_HOME)/Src/Boundary/Make.package
include $(AMREX_HOME)/Src/Particle/Make.package
include $(AMREX_HOME)/Tools/GNUMake/Make.rules