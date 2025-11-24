#!/bin/sh

#DSUB --job_type cosched
#DSUB -n case_channelflow
#DSUB -A root.huazkjdxmrsgjzdsyshi
#DSUB -q root.default
#DSUB -R cpu=128;mem=41960;gpu=1
#DSUB -N 1
#DSUB -o %J-out.log
#DSUB -e %J-out.log


#EXE = 'main3d.gnu.MPI.CUDA.ex'

#===========================================================
#加载环境变量
#===========================================================
source /home/HPCBase/tools/module-5.2.0/init/profile.sh
module use /home/HPCBase/modulefiles/
module purge
module load mpi/hmpi/1.2.0_bs2.4.0_sp1
module load compilers/cuda/11.7.0
module load compilers/gcc/10.3.1
#===========================================================
#获得hostfile
#===========================================================
echo ----- print env vars -----

if [ "${CCS_ALLOC_FILE}" != "" ]; then
    echo "   "
    ls -la ${CCS_ALLOC_FILE}
    echo ------ cat ${CCS_ALLOC_FILE}
    cat ${CCS_ALLOC_FILE}
fi

export HOSTFILE=/tmp/hostfile.$$
rm -rf $HOSTFILE
touch $HOSTFILE


ntask=`cat ${CCS_ALLOC_FILE} | awk -v fff="$HOSTFILE" '{}
{
    split($0, a, " ")
    if (length(a[1]) >0 && length(a[3]) >0) {
        print a[1]" slots="a[2] >> fff
        total_task+=a[3]
    }
}END{print total_task}'`

echo "openmpi hostfile $HOSTFILE generated:"
echo "-----------------------"
cat $HOSTFILE
echo "-----------------------"
echo "Total tasks is $ntask"
echo "mpirun -hostfile $HOSTFILE -n $ntask <your application>"

#===========================================================
#运行测试脚本
#===========================================================

mpirun -hostfile $HOSTFILE -npernode 4 -x PATH -x LD_LIBRARY_PATH  --mca plm_rsh_agent /opt/batch/agent/tools/dstart ./main3d.gnu.MPI.CUDA.ex config/inputs

ret=$?

#rm -rf $HOSTFILE
#exit $ret
