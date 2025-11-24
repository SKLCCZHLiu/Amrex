#!/usr/bin/env bash
# 简单诊断脚本：检测 clangd 能否正确索引 AMReX 目标函数定义文件
# 用法: ./diagnose_clangd.sh

set -e
ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
AMREX_SRC="/home/huazkjdxmrsgjzdsyshi/whcs-share18/wangyan/amrex-23.09/Src"
CCDB="$ROOT_DIR/compile_commands.json"
TARGET_FILE="$AMREX_SRC/AmrCore/AMReX_AmrCore.cpp"

echo "[INFO] 工作区根目录: $ROOT_DIR"
[[ -f "$CCDB" ]] && echo "[OK] compile_commands.json 存在" || { echo "[FAIL] 缺少 compile_commands.json"; exit 1; }

# 检查 compile_commands.json 是否包含目标文件
if grep -q "AMReX_AmrCore.cpp" "$CCDB"; then
  echo "[OK] 编译数据库包含 AMReX_AmrCore.cpp"
else
  echo "[WARN] 编译数据库未列出 AMReX_AmrCore.cpp，clangd 跳转会失败"
fi

# 简单统计 AMReX 源码文件数
COUNT=$(find "$AMREX_SRC/AmrCore" -maxdepth 1 -name '*.cpp' | wc -l)
echo "[INFO] AmrCore 模块 cpp 文件数量: $COUNT"

# 检查 .clangd 是否含有 AmrCore include
if grep -q "AmrCore" "$ROOT_DIR/.clangd"; then
  echo "[OK] .clangd 中包含 AmrCore 目录 -I 设置"
else
  echo "[WARN] .clangd 未显式包含 AmrCore 目录 -I"
fi

# 建议操作提示
cat <<EOF

下一步建议：
1. VSCode 命令面板执行: Clangd: Restart language server
2. 打开 AMReX_AmrCore.H 后再次尝试跳转 InitFromScratch
3. 若仍失败，在 clangd.arguments 中临时加入 --log=verbose 并查看输出面板
EOF
