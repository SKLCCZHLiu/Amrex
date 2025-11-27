#!/usr/bin/env bash
set -Eeuo pipefail


# 在下方直接修改源路径与目标路径即可使用。
# 优先严格按 .gitignore 语义（git 文件清单）复制；不在 git 仓库内时回退到 rsync 的 per-dir .gitignore 过滤。

# ===== 配置区域（请按需修改） =====
# 源目录（默认：脚本所在目录）
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)
SOURCE_DIR="/home/huazkjdxmrsgjzdsyshi/whcs-share18/caiyimin/learnamerx/Amrex/projects/Cylinder2D_ori"   # <--- 修改为你希望复制的源目录绝对路径

# 目标目录（最终落地目录名，可不存在，会被创建）
DEST_DIR="/home/huazkjdxmrsgjzdsyshi/whcs-share18/caiyimin/learnamerx/Amrex/projects/Cylinder2D_noBTDF"  # <--- 修改为你希望复制到的目标目录绝对路径

# 注意：该脚本会实际复制（无 dry-run/删除开关）；如需试运行建议临时手动在 rsync 命令上添加 -n。
# ===== 配置结束 =====

if ! command -v rsync >/dev/null 2>&1; then
  echo "错误: 未找到 rsync，请安装后重试。" >&2
  exit 1
fi

if [[ ! -d "$SOURCE_DIR" ]]; then
  echo "错误: 源目录不存在: $SOURCE_DIR" >&2
  exit 1
fi

mkdir -p "$(dirname -- "$DEST_DIR")"
mkdir -p "$DEST_DIR"

echo "源: $SOURCE_DIR"
echo "目标: $DEST_DIR"

# 优先使用 git 文件清单，严格按 .gitignore 语义处理（包含未被忽略的未跟踪文件，支持 ! 反选）
if command -v git >/dev/null 2>&1 && git -C "$SOURCE_DIR" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
  REPO_ROOT=$(git -C "$SOURCE_DIR" rev-parse --show-toplevel)
  # 计算 SOURCE_DIR 相对于仓库根的路径
  if command -v realpath >/dev/null 2>&1; then
    SOURCE_REL=$(realpath --relative-to="$REPO_ROOT" "$SOURCE_DIR")
  else
    if command -v python3 >/dev/null 2>&1; then
      SOURCE_REL=$(python3 - <<'PY'
import os
repo=os.path.realpath(os.environ['REPO'])
src=os.path.realpath(os.environ['SRC'])
print(os.path.relpath(src, repo))
PY
REPO="$REPO_ROOT" SRC="$SOURCE_DIR")
    else
      SOURCE_REL="${SOURCE_DIR#"$REPO_ROOT/"}"
    fi
  fi

  WORK_DIR="$REPO_ROOT/$SOURCE_REL"
  RSYNC_OPTS=(-a -v -m --exclude '.git/')

  echo "模式: 使用 git 文件清单"
  # 仅同步 git 识别为未忽略的文件（跟踪 + 未被忽略的未跟踪）；并过滤掉工作区中不存在的路径
  tmp_list=$(mktemp)
  trap 'rm -f "$tmp_list"' EXIT
  git -C "$WORK_DIR" ls-files -z --cached --others --exclude-standard -- . \
    | { while IFS= read -r -d '' f; do
          if [[ -e "$WORK_DIR/$f" ]]; then
            printf '%s\0' "$f"
          fi
        done > "$tmp_list"; } || true

  if [[ -s "$tmp_list" ]]; then
    rsync "${RSYNC_OPTS[@]}" --files-from="$tmp_list" --from0 "$WORK_DIR/" "$DEST_DIR/"
    echo "完成：$DEST_DIR"
    exit 0
  else
    echo "提示: git 文件清单为空，回退到 rsync + .gitignore 模式。"
    # 回退：使用 rsync 的 per-dir .gitignore 过滤（不完美，不支持 ! 反选）
    RSYNC_OPTS=(-a -v -m --exclude '.git/' --filter ":- .gitignore")
    rsync "${RSYNC_OPTS[@]}" "$SOURCE_DIR/" "$DEST_DIR/"
    echo "完成：$DEST_DIR"
    exit 0
  fi
fi

# 回退：使用 rsync 的 per-dir .gitignore 过滤（不完美，不支持 ! 反选）
RSYNC_OPTS=(-a -v -m --exclude '.git/' --filter ":- .gitignore")

echo "模式: 使用 rsync + .gitignore（回退）"
rsync "${RSYNC_OPTS[@]}" "$SOURCE_DIR/" "$DEST_DIR/"

echo "完成：$DEST_DIR"
