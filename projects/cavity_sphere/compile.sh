#!/bin/bash
# 自动编译脚本 - 用于在编译节点上编译代码

# 检查是否在编译节点上
if [[ $(hostname) != "whshare-agent-1" ]]; then
    echo "正在连接到编译节点 whshare-agent-1..."
    ssh whshare-agent-1 << 'EOF'
        cd /home/share/huazkjdxmrsgjzdsyshi/home/lijing/caiyimin/learnamerx/cavity_sphere
        
        # 加载环境
        module purge
        source /home/HPCBase/tools/module-5.2.0/init/profile.sh
        module use /home/HPCBase/modulefiles/
        module load mpi/hmpi/1.2.0_bs2.4.0_sp1
        module load compilers/cuda/12.1.0
        module load compilers/gcc/10.3.1
        
        echo "环境已加载，开始编译..."
        make -j8 CUDA_ARCH=80
        
        if [ $? -eq 0 ]; then
            echo "编译成功！"
        else
            echo "编译失败，请检查错误信息。"
        fi
EOF
else
    echo "已在编译节点上，直接编译..."
    
    # 加载环境
    module purge
    source /home/HPCBase/tools/module-5.2.0/init/profile.sh
    module use /home/HPCBase/modulefiles/
    module load mpi/hmpi/1.2.0_bs2.4.0_sp1
    module load compilers/cuda/12.1.0
    module load compilers/gcc/10.3.1
    
    echo "环境已加载，开始编译..."
    make -j8 CUDA_ARCH=80
    
    if [ $? -eq 0 ]; then
        echo "编译成功！"
    else
        echo "编译失败，请检查错误信息。"
    fi
fi