#!/bin/sh

#DSUB --job_type cosched
#DSUB -n case_channelflow_strace
#DSUB -A root.huazkjdxmrsgjzdsyshi
#DSUB -q root.default
#DSUB -R cpu=64;mem=49152;gpu=2
#DSUB -N 1
#DSUB -o %J-out.log
#DSUB -e %J-out.log

#===========================================================
# 加载运行环境
#===========================================================
source /home/HPCBase/tools/module-5.2.0/init/profile.sh
module use /home/HPCBase/modulefiles/
module purge
module load mpi/hmpi/1.2.0_bs2.4.0_sp1
module load compilers/cuda/11.7.0
module load compilers/gcc/10.3.1

#===========================================================
# 生成 hostfile
#===========================================================
echo ----- print env vars -----
if [ -n "${CCS_ALLOC_FILE}" ]; then
    echo "   "
    ls -la "${CCS_ALLOC_FILE}"
    echo ------ cat "${CCS_ALLOC_FILE}"
    cat "${CCS_ALLOC_FILE}"
fi

export HOSTFILE=/tmp/hostfile.$$
rm -f "$HOSTFILE"
touch "$HOSTFILE"

ntask=$(awk -v fff="$HOSTFILE" '
{
    split($0, a, " ")
    if (length(a[1]) > 0 && length(a[2]) > 0) {
        print a[1]" slots="a[2] >> fff
    }
    if (length(a[3]) > 0) {
        total_task += a[3]
    }
}
END { print total_task }' "${CCS_ALLOC_FILE}")

if [ -z "$ntask" ]; then
    ntask=1
fi

echo "openmpi hostfile $HOSTFILE generated:"
echo "-----------------------"
cat "$HOSTFILE"
echo "-----------------------"
echo "Total tasks is $ntask"
echo "mpirun -hostfile $HOSTFILE -n $ntask <your application>"

#===========================================================
# GPU / rank 拓扑
#===========================================================
NGPUS_PER_NODE=1
if [ -n "$CUDA_VISIBLE_DEVICES" ]; then
    NGPUS_PER_NODE=$(echo "$CUDA_VISIBLE_DEVICES" | awk -F',' '{print NF}')
elif command -v nvidia-smi >/dev/null 2>&1; then
    NGPUS_PER_NODE=$(nvidia-smi -L 2>/dev/null | wc -l)
fi
if [ -z "$NGPUS_PER_NODE" ] || [ "$NGPUS_PER_NODE" -lt 1 ]; then
    NGPUS_PER_NODE=1
fi

echo "Detected GPUs per node: $NGPUS_PER_NODE"

NNODES=$(grep -c . "$HOSTFILE")
if [ -z "$NNODES" ] || [ "$NNODES" -lt 1 ]; then
    NNODES=1
fi
TOTAL_RANKS=$((NNODES * NGPUS_PER_NODE))
if [ "$TOTAL_RANKS" -lt 1 ]; then
    TOTAL_RANKS=1
fi
OTHER_RANKS=$((TOTAL_RANKS - 1))

STRACE_FILTER="open,openat,close,write,pwrite64,rename,unlink,fsync,fdatasync"
STRACE_PREFIX="strace.rank0"

MPI_COMMON_ARGS="-hostfile $HOSTFILE \
    -x PATH -x LD_LIBRARY_PATH \
    --map-by ppr:${NGPUS_PER_NODE}:node \
    --mca plm_rsh_agent /opt/batch/agent/tools/dstart"

if [ "$OTHER_RANKS" -gt 0 ]; then
        mpirun $MPI_COMMON_ARGS \
            -n 1 bash -lc 'export CUDA_VISIBLE_DEVICES=${OMPI_COMM_WORLD_LOCAL_RANK:-${MPI_LOCALRANKID:-0}}; \
                exec strace -ff -ttT -o '"${STRACE_PREFIX}"' -e trace='"${STRACE_FILTER}"' \
                ./main3d.gnu.MPI.CUDA.ex config/inputs' \
            : -n "$OTHER_RANKS" bash -lc 'export CUDA_VISIBLE_DEVICES=${OMPI_COMM_WORLD_LOCAL_RANK:-${MPI_LOCALRANKID:-0}}; \
                exec ./main3d.gnu.MPI.CUDA.ex config/inputs'
else
        mpirun $MPI_COMMON_ARGS \
            -n 1 bash -lc 'export CUDA_VISIBLE_DEVICES=${OMPI_COMM_WORLD_LOCAL_RANK:-${MPI_LOCALRANKID:-0}}; \
                exec strace -ff -ttT -o '"${STRACE_PREFIX}"' -e trace='"${STRACE_FILTER}"' \
                ./main3d.gnu.MPI.CUDA.ex config/inputs'
fi

ret=$?

#rm -f "$HOSTFILE"
exit $ret