#!/bin/bash
# VSCode C++ 配置一键切换脚本
# 用法: ./switch_config.sh clangd|cpptools

set -e
MODE="$1"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$SCRIPT_DIR/.."
VSCODE_DIR="$ROOT/ChannelFlow/.vscode"
WORKSPACE_FILE="$ROOT/learnamerx.code-workspace"

if [[ "$MODE" != "clangd" && "$MODE" != "cpptools" ]]; then
  echo "[ERROR] 用法: $0 clangd|cpptools"
  exit 1
fi

echo "[INFO] 正在切换到 $MODE 配置, 原有相关配置将被删除并替换..."

# 删除原有配置
rm -f "$ROOT/.clangd" "$ROOT/compile_commands.json"
rm -f "$VSCODE_DIR/c_cpp_properties.json" "$VSCODE_DIR/settings.json"
rm -f "$WORKSPACE_FILE"
rm -f "$ROOT/.vscode/browse.vc.db" 2>/dev/null || true
rm -rf "$ROOT/.vscode/ipch" 2>/dev/null || true

# 复制新配置
cp "$ROOT/dev_profiles/$MODE/.vscode/settings.json" "$VSCODE_DIR/settings.json" 2>/dev/null || true
cp "$ROOT/dev_profiles/$MODE/c_cpp_properties.json" "$VSCODE_DIR/c_cpp_properties.json" 2>/dev/null || true
cp "$ROOT/dev_profiles/$MODE/.clangd" "$ROOT/.clangd" 2>/dev/null || true
cp "$ROOT/dev_profiles/$MODE/compile_commands.json" "$ROOT/compile_commands.json" 2>/dev/null || true
cp "$ROOT/dev_profiles/$MODE/learnamerx.code-workspace" "$WORKSPACE_FILE" 2>/dev/null || true

# 提示完成
if [[ "$MODE" == "clangd" ]]; then
  # 创建符号链接便于统一路径浏览（忽略错误：已存在）
  mkdir -p "$ROOT/external" 2>/dev/null || true
  ln -sfn /home/huazkjdxmrsgjzdsyshi/whcs-share18/wangyan/amrex-23.09/Src "$ROOT/external/amrex"
  echo "[INFO] 已清理 cpptools 索引缓存 (browse.vc.db / ipch)。"
  echo "[SUCCESS] 已切换到 clangd 配置。重新打开 workspace 后等待后台索引完成 (status bar spinner)。"
else
  echo "[INFO] 已清理旧的 cpptools 索引缓存，clangd 专用文件已移除。"
  echo "[SUCCESS] 已切换到 cpptools 配置。重新打开 workspace 后等待 Parsing / IntelliSense 完成。"
fi
