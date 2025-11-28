#!/bin/bash
# 自动编译脚本 - 固定在编译节点 whshare-agent-1 上构建 AMReX/LBM-AMR 项目
#
# 模块化结构速览（Option 3 文档说明，仅注释，不改变行为）：
#   ─ 工具/环境函数
#       • find_project_root(): 解析项目根目录
#       • load_environment(): HPC module 初始化
#   ─ 编译执行路径
#       • do_compile(): 负责编译及 compile_commands.json 生成
#       • verify_artifacts(): 编译结果检测
#       • run_here()/ssh 调度：控制本地或远端执行
#   ─ 辅助逻辑
#       • 日志管理：LOG_* 环境变量、rotate_logs()、generate_summary()
#       • 编译数据库包装器：在 do_compile() 内定义 WRAPDIR/bin/gcc/g++/nvcc/git
#
# 未来若需进一步模块化，可考虑拆分为 scripts/lib/{utils,env,logging,ccdb,build}.sh，并将包装器脚本独立存放。
#
# 使用说明（环境变量，可选）：
#   - MAKE_J   : make 并行度（默认 8）
#       例：MAKE_J=16 ./compile.sh
#   - CUDA_ARCH: 传递给 Make 的 CUDA 架构号（默认 80）
#       例：CUDA_ARCH=86 ./compile.sh
#   - GEN_CCDB : 是否生成编译数据库 compile_commands.json（0 不生成；1 生成；默认 1）
#       例：GEN_CCDB=0 ./compile.sh   # 跳过生成编译数据库，构建更快
#   - CCDB_METHOD: 生成编译数据库的方法（auto|wrapper|bear|intercept，默认 auto）
#       例：CCDB_METHOD=wrapper GEN_CCDB=1 ./compile.sh  # 强制使用包装器生成“干净”的 compile_commands.json
#
# 运行逻辑：
#   - 如果当前主机名不是 whshare-agent-1，则通过 SSH 在 whshare-agent-1 上执行本脚本；
#   - 如果已经在 whshare-agent-1 上，则直接加载环境并构建；
#   - 若启用编译数据库生成，优先使用 bear 或 intercept-build；若都不存在，则自动用 gcc/g++ 包装器采集命令生成 compile_commands.json。
set -euo pipefail

# 可配置项（通过环境变量覆盖）
MAKE_J="${MAKE_J:-16}"
CUDA_ARCH="${CUDA_ARCH:-80}"
GEN_CCDB="${GEN_CCDB:-0}"
# 生成编译数据库的方法控制（auto|wrapper|bear|intercept）
CCDB_METHOD="${CCDB_METHOD:-wrapper}"
# 是否在构建遇错时继续尽量编译后续目标（有助于捕获更多编译命令，生成更完整的 CCDB）
MAKE_KEEP_GOING="${MAKE_KEEP_GOING:-0}"

# 控制编译数据库“胖/瘦”的可选开关：
#  - CCDB_KEEP_TMP=1    保留 nvcc 产生的临时 tmpxft_/cudafe* 记录（默认 0，不保留）
#  - CCDB_DEDUP_BY_FILE=0 禁用按 file 去重（默认 1，启用去重）
CCDB_KEEP_TMP="${CCDB_KEEP_TMP:-0}"
CCDB_DEDUP_BY_FILE="${CCDB_DEDUP_BY_FILE:-1}"

# 简单日志函数（注意：必须在首次调用前定义）
info() { echo "[$(date '+%F %T')] $*"; }

# 解析脚本绝对路径（若通过 STDIN 执行则可能获取不到）
if [[ -n "${BASH_SOURCE[0]:-}" && -f "${BASH_SOURCE[0]}" ]]; then
    if command -v realpath >/dev/null 2>&1; then
        _CF_SCRIPT_PATH="$(realpath "${BASH_SOURCE[0]}")"
    else
        _CF_SCRIPT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"
    fi
else
    _CF_SCRIPT_PATH=""
fi

find_project_root() {
    local dir="$1"
    while [[ -n "$dir" && "$dir" != "/" ]]; do
        if [[ -f "$dir/GNUmakefile" ]]; then
            if [[ -d "$dir/src" || -f "$dir/Make.package" || -d "$dir/config" ]]; then
                printf '%s\n' "$dir"
                return 0
            fi
        fi
        dir="$(dirname "$dir")"
    done
    return 1
}

if [[ -n "${_CHANNELFLOW_PROJECT_ROOT:-}" ]]; then
    PROJECT_ROOT="${_CHANNELFLOW_PROJECT_ROOT}"
else
    start_dir=""
    if [[ -n "$_CF_SCRIPT_PATH" ]]; then
        start_dir="$(dirname "$_CF_SCRIPT_PATH")"
    else
        start_dir="$(pwd)"
    fi
    if ! PROJECT_ROOT=$(find_project_root "$start_dir"); then
        if ! PROJECT_ROOT=$(find_project_root "$(pwd)"); then
            echo "错误: 未能定位 ChannelFlow 项目根目录 (起始路径: $start_dir)" >&2
            exit 1
        fi
    fi
    export _CHANNELFLOW_PROJECT_ROOT="$PROJECT_ROOT"
fi

PROJECT_ROOT="$(cd "$PROJECT_ROOT" && pwd -P)"
cd "$PROJECT_ROOT"

    # --- 可选日志重定向（将 stdout/stderr 写入日志文件）
    # 控制变量:
    #   LOG_TO_FILE=1|0         是否将输出写入文件（默认 1）
    #   LOG_TEE=1|0             是否同时在终端显示（使用 tee）（默认 1，即同时显示）
    #   LOG_DIR=...             日志目录（默认 $PROJECT_ROOT/logs）
    #   LOG_MAX_FILES=N         保留最近 N 个日志（默认 20）
    #   LOG_SUMMARY=1|0         是否在编译结束时生成错误/警告摘要（默认 1）
    #   LOG_SUMMARY_LINES=N     摘要中保留的最大行数（默认 200）
    #   LOG_ERRORS_PATTERN      grep 用的模式（默认: error|warning|undefined reference|fatal）
    LOG_TO_FILE="${LOG_TO_FILE:-1}"
    LOG_TEE="${LOG_TEE:-1}"
    LOG_DIR="${LOG_DIR:-$PROJECT_ROOT/logs}"
    LOG_MAX_FILES="${LOG_MAX_FILES:-3}"
    LOG_SUMMARY="${LOG_SUMMARY:-1}"
    LOG_SUMMARY_LINES="${LOG_SUMMARY_LINES:-200}"
    LOG_ERRORS_PATTERN="error:|Error |ERROR |error |Error:|FATAL:|fatal |undefined reference|cannot find|No such file|Permission denied|Segmentation fault|cannot open|make: \*\*\*|failed|FAILED"

    generate_summary() {
    # 在退出时生成单独的 summary 文件，便于快速查看关键错误/警告
    if [[ "$LOG_TO_FILE" != "1" || "$LOG_SUMMARY" != "1" ]]; then
        return 0
    fi
    if [[ -z "$LOGFILE" || ! -f "$LOGFILE" ]]; then
        return 0
    fi
    summary_file="${LOGFILE%.log}-summary.txt"
    
    # 更全面的错误模式
    LOG_ERRORS_PATTERN="error:|Error |ERROR |error |Error:|FATAL:|fatal |undefined reference|cannot find|No such file|Permission denied|Segmentation fault|cannot open|make: \*\*\*|failed|FAILED"
    
    # 创建分类错误摘要
    echo "=== 编译错误 ===" > "$summary_file"
    grep -Ei "error:|Error |ERROR |error |Error:|FATAL:|fatal " -A 2 -B 2 "$LOGFILE" | head -n "$LOG_SUMMARY_LINES" >> "$summary_file"
    
    echo -e "\n=== 链接错误 ===" >> "$summary_file"
    grep -Ei "undefined reference|cannot find -l" -A 2 -B 2 "$LOGFILE" | head -n "$LOG_SUMMARY_LINES" >> "$summary_file"
    
    echo -e "\n=== 文件错误 ===" >> "$summary_file"
    grep -Ei "No such file|cannot open" -A 2 -B 2 "$LOGFILE" | head -n "$LOG_SUMMARY_LINES" >> "$summary_file"
    
    echo -e "\n=== 警告信息 ===" >> "$summary_file"
    grep -Ei "warning:|Warning |WARNING " -A 2 -B 2 "$LOGFILE" | head -n "$LOG_SUMMARY_LINES" >> "$summary_file"
    
    # 添加错误统计
    echo -e "\n=== 错误统计 ===" >> "$summary_file"
    echo "编译错误: $(grep -ci "error:" "$LOGFILE")" >> "$summary_file"
    echo "链接错误: $(grep -ci "undefined reference" "$LOGFILE")" >> "$summary_file"
    echo "警告信息: $(grep -ci "warning:" "$LOGFILE")" >> "$summary_file"
    
    # 如果没有关键错误，显示日志末尾
    if ! grep -Ei "$LOG_ERRORS_PATTERN" "$LOGFILE" >/dev/null 2>&1; then
        echo -e "\n=== 日志末尾 ===" >> "$summary_file"
        tail -n "$LOG_SUMMARY_LINES" "$LOGFILE" >> "$summary_file"
    fi
    
    ln -sf "$summary_file" "${LOG_DIR}/compile-latest-summary.txt" || true
    info "已生成分类日志摘要: $summary_file"
}

    rotate_logs() {
        # 保留最近 $LOG_MAX_FILES 个 compile-*.log，删除更旧的
        if [[ ! -d "$LOG_DIR" ]]; then
            return 0
        fi
        mapfile -t files < <(ls -1t "$LOG_DIR"/compile-*.log 2>/dev/null || true)
        if [[ ${#files[@]} -le $LOG_MAX_FILES ]]; then
            return 0
        fi
        # 删除索引 >= LOG_MAX_FILES
        for ((i=LOG_MAX_FILES; i<${#files[@]}; i++)); do
            rm -f "${files[$i]}" || true
        done
    }

    if [[ "$LOG_TO_FILE" == "1" ]]; then
        mkdir -p "$LOG_DIR"
        _cf_ts="$(date '+%Y%m%dT%H%M%S')"
        LOGFILE="$LOG_DIR/compile-${_cf_ts}.log"
        # 先进行轮转，再创建新文件
        rotate_logs
        # 保持一个指向最新日志的符号链接（摘要会在退出时生成）
        ln -sf "$LOGFILE" "$LOG_DIR/compile-latest.log" || true
        # 在退出时生成摘要（会写入独立 summary 文件）
        trap generate_summary EXIT
        if [[ "$LOG_TEE" == "1" ]]; then
            # 将 stdout/stderr 通过 tee 写入文件并在终端显示
            exec > >(tee -a "$LOGFILE") 2>&1
        else
            # 只写入日志文件，不在终端显示
            exec > "$LOGFILE" 2>&1
        fi
        info "日志输出已重定向到: $LOGFILE"
    else
        info "LOG_TO_FILE=0: 未启用日志文件写入，输出仍在终端显示。"
    fi

SCRIPT_PAYLOAD="${_CF_SCRIPT_PATH:-$0}"

# 1) 加载 HPC 模块环境（确保 module 命令可用，修正顺序：先 init 再 purge/load）
load_environment() {
    if ! command -v module >/dev/null 2>&1; then
        if [ -f /home/HPCBase/tools/module-5.2.0/init/profile.sh ]; then
            # shellcheck disable=SC1091
            source /home/HPCBase/tools/module-5.2.0/init/profile.sh
        elif [ -f /etc/profile.d/modules.sh ]; then
            # shellcheck disable=SC1091
            source /etc/profile.d/modules.sh
        fi
    fi

    module purge || true
    module use /home/HPCBase/modulefiles/ || true
    module load mpi/hmpi/1.2.0_bs2.4.0_sp1 || true
    module load compilers/cuda/12.1.0 || true
    module load compilers/gcc/10.3.1 || true

    # 基础工具提示
    command -v nvcc >/dev/null 2>&1 || echo "警告: nvcc 未找到，可能无法进行 CUDA 构建。" >&2
    command -v mpirun >/dev/null 2>&1 || echo "警告: mpirun 未找到，MPI 可能不可用。" >&2
}

# 2) 执行编译并（可选）生成 compile_commands.json
do_compile() {
    local make_status=0
    local make_cmd=( make -j"${MAKE_J}" CUDA_ARCH="${CUDA_ARCH}" )
    if [ "${MAKE_KEEP_GOING}" = "1" ]; then
        make_cmd+=( -k )
    fi

    if [ "${GEN_CCDB}" = "1" ]; then
        # 选择生成编译数据库的方式
        use_wrapper=false
        case "${CCDB_METHOD}" in
            wrapper) use_wrapper=true ;;
            bear)    if command -v bear >/dev/null 2>&1; then
                         info "使用 bear 捕获编译数据库..."; bear -- "${make_cmd[@]}" || make_status=$? ;
                      else
                         info "未找到 bear，回退到 wrapper 方法。"; use_wrapper=true ;
                      fi ;;
            intercept) if command -v intercept-build >/dev/null 2>&1; then
                          info "使用 intercept-build 捕获编译数据库...";
                          intercept-build --override-compiler --output compile_commands.json "${make_cmd[@]}" || make_status=$?
                        else
                          info "未找到 intercept-build，回退到 wrapper 方法。"; use_wrapper=true ;
                        fi ;;
            auto|*)
                      if command -v bear >/dev/null 2>&1; then
                          info "使用 bear 捕获编译数据库..."; bear -- "${make_cmd[@]}" || make_status=$?
                      elif command -v intercept-build >/dev/null 2>&1; then
                          info "使用 intercept-build 捕获编译数据库...";
                          intercept-build --override-compiler --output compile_commands.json "${make_cmd[@]}" || make_status=$?
                      else
                          use_wrapper=true
                      fi ;;
        esac

        if [ "${use_wrapper}" = true ]; then
            info "未检测到 bear/intercept-build，使用 PATH 包装 gcc/g++/nvcc 写入 JSON。"
            rm -f compile_commands.json .cc_commands.jsonl .cxx_commands.jsonl .nvcc_commands.jsonl
            local WRAPDIR="$(pwd)/.wrapbin"
            mkdir -p "${WRAPDIR}"

            # 优先使用当前已激活的编译器，若找不到则回退到固定路径
            export REAL_GCC="${REAL_GCC:-$(command -v gcc || echo /home/HPCBase/compilers/gcc/10.3.1/bin/gcc)}"
            export REAL_GXX="${REAL_GXX:-$(command -v g++ || echo /home/HPCBase/compilers/gcc/10.3.1/bin/g++)}"
            export REAL_GIT="${REAL_GIT:-$(command -v git || echo /usr/bin/git)}"
            export REAL_NVCC="${REAL_NVCC:-$(command -v nvcc || echo /usr/local/cuda/bin/nvcc)}"
            export REAL_MPICXX="${REAL_MPICXX:-$(command -v mpicxx || echo /home/HPCBase/mpi/hmpi/1.2.0-huawei-sp1/bin/mpicxx)}"
            export REAL_MPICC="${REAL_MPICC:-$(command -v mpicc || echo /home/HPCBase/mpi/hmpi/1.2.0-huawei-sp1/bin/mpicc)}"

            cat > "${WRAPDIR}/gcc" <<'EOS'
#!/usr/bin/env bash
out=.cc_commands.jsonl
args=("$@")
has_nvcc=0
has_c=0
for a in "${args[@]}"; do
    [[ "$a" == "-c" ]] && has_c=1
    [[ "$a" == --expt-* || "$a" == "--forward-unknown-to-host-compiler" || "$a" == -Xcudafe* || "$a" == -Xptxas* || "$a" == --diag_suppress* ]] && has_nvcc=1
done
# strip dependency-gen flags
filtered=()
skip_next=0
for ((i=0;i<${#args[@]};i++)); do
    if (( skip_next )); then skip_next=0; continue; fi
    case "${args[$i]}" in
        -MMD|-MP) continue ;;
        -MF) skip_next=1; continue ;;
    esac
    filtered+=("${args[$i]}")
done
json_escape() {
    local s="$1"
    s="${s//\\/\\\\}"; s="${s//\"/\\\"}"; s="${s//$'\n'/}"
    printf "%s" "$s"
}
if (( has_nvcc==0 && has_c==1 )); then
    src=""
    for a in "${filtered[@]}"; do
        case "$a" in *.c|*.cc|*.cpp|*.cxx|*.C) src="$a" ;; esac
    done
    # 过滤 nvcc 生成的临时中间文件（例如 /tmp/tmpxft_* 或 *cudafe*.cpp），避免污染编译数据库
    if [[ -n "$src" ]]; then
        bname="$(basename -- "$src")"
        if [[ "$src" == /tmp/tmpxft_* ]] || [[ "$bname" == tmpxft_* ]] || [[ "$src" == *cudafe* ]]; then
            exec "$REAL_GCC" "${filtered[@]}"
        fi
    fi
    args_json="\"$(json_escape "$REAL_GCC")\""
    for a in "${filtered[@]}"; do args_json+=",\"$(json_escape "$a")\""; done
    printf '{"directory":"%s","arguments":[%s],"file":"%s"}\n' "$(pwd)" "$args_json" "$src" >> "$out"
fi
exec "$REAL_GCC" "${filtered[@]}"
EOS

            cat > "${WRAPDIR}/g++" <<'EOS'
#!/usr/bin/env bash
out=.cxx_commands.jsonl
args=("$@")
has_nvcc=0
has_c=0
for a in "${args[@]}"; do
    [[ "$a" == "-c" ]] && has_c=1
    [[ "$a" == --expt-* || "$a" == "--forward-unknown-to-host-compiler" || "$a" == -Xcudafe* || "$a" == -Xptxas* || "$a" == --diag_suppress* ]] && has_nvcc=1
done
filtered=()
skip_next=0
for ((i=0;i<${#args[@]};i++)); do
    if (( skip_next )); then skip_next=0; continue; fi
    case "${args[$i]}" in
        -MMD|-MP) continue ;;
        -MF) skip_next=1; continue ;;
    esac
    filtered+=("${args[$i]}")
done
json_escape() {
    local s="$1"
    s="${s//\\/\\\\}"; s="${s//\"/\\\"}"; s="${s//$'\n'/}"
    printf "%s" "$s"
}
if (( has_nvcc==0 && has_c==1 )); then
    src=""
    for a in "${filtered[@]}"; do
        case "$a" in *.c|*.cc|*.cpp|*.cxx|*.C) src="$a" ;; esac
    done
    # 过滤 nvcc 生成的临时中间文件（例如 /tmp/tmpxft_* 或 *cudafe*.cpp），避免污染编译数据库
    if [[ -n "$src" ]]; then
        bname="$(basename -- "$src")"
        if [[ "$src" == /tmp/tmpxft_* ]] || [[ "$bname" == tmpxft_* ]] || [[ "$src" == *cudafe* ]]; then
            exec "$REAL_GXX" "${filtered[@]}"
        fi
    fi
    args_json="\"$(json_escape "$REAL_GXX")\""
    for a in "${filtered[@]}"; do args_json+=",\"$(json_escape "$a")\""; done
    printf '{"directory":"%s","arguments":[%s],"file":"%s"}\n' "$(pwd)" "$args_json" "$src" >> "$out"
fi
exec "$REAL_GXX" "${filtered[@]}"
EOS

                        # nvcc 包装器：尝试将 nvcc 的编译步骤转换为等效的主机编译器调用记录，便于 clangd 解析
                        cat > "${WRAPDIR}/nvcc" <<'EOS'
#!/usr/bin/env bash
out=.nvcc_commands.jsonl
args=("$@")

# 判断是否是编译步骤 (-c 存在)；收集源文件
has_c=0; src=""
for a in "${args[@]}"; do
    [[ "$a" == "-c" ]] && has_c=1
    case "$a" in *.c|*.cc|*.cpp|*.cxx|*.C|*.cu) src="$a" ;; esac
done

# 解析 -ccbin 指定的主机编译器
HOST_CXX="${REAL_GXX:-$(command -v g++ || echo g++)}"
for ((i=0;i<${#args[@]};i++)); do
    a="${args[$i]}"
    case "$a" in
        -ccbin=*) HOST_CXX="${a#-ccbin=}" ;;
        -ccbin)
            if (( i+1 < ${#args[@]} )); then HOST_CXX="${args[$((i+1))]}"; fi ;;
    esac
done

## 提取有助于主机解析的参数：-I/-isystem（含分离形式）/-D/-std/-O*/-g 以及 -Xcompiler 转发
host_flags=()
have_std=0
for ((i=0;i<${#args[@]};i++)); do
    a="${args[$i]}"
    case "$a" in
        # 粘连形式：-I<path> 或 -isystem<path>
        -I*|-isystem*|-D*|-O*|-g|-f*) host_flags+=("$a") ;;
        # 分离形式：-I <path> 或 -isystem <path>
        -I|-isystem)
            if (( i+1 < ${#args[@]} )); then
                nxt="${args[$((i+1))]}"
                if [[ -n "$nxt" && ! "$nxt" =~ ^- ]]; then
                    host_flags+=("$a" "$nxt")
                    ((i++))
                    continue
                fi
            fi
            ;;
        # 仅选择安全的 -m* 选项，避免将 -maxrregcount 等 CUDA 专属参数错误纳入
        -m32|-m64|-march=*|-mtune=*|-mcpu=*|-mfpu=*|-mno-*|-mavx*|-msse*|-mfma*) host_flags+=("$a") ;;
        -maxrregcount|-maxrregcount=*) ;; # 显式忽略
        -std=*) have_std=1; host_flags+=("$a") ;;
        -Xcompiler)
            if (( i+1 < ${#args[@]} )); then
                IFS=',' read -r -a xc <<< "${args[$((i+1))]}"
                for x in "${xc[@]}"; do [[ -n "$x" ]] && host_flags+=("$x"); done
            fi ;;
        -Xcompiler=*)
            IFS=',' read -r -a xc <<< "${a#-Xcompiler=}"
            for x in "${xc[@]}"; do [[ -n "$x" ]] && host_flags+=("$x"); done ;;
    esac
done
[[ $have_std -eq 0 ]] && host_flags+=("-std=c++17")

# 过滤依赖生成参数
filtered=()
skip_next=0
for ((i=0;i<${#args[@]};i++)); do
    if (( skip_next )); then skip_next=0; continue; fi
    case "${args[$i]}" in -MMD|-MP) continue ;; -MF) skip_next=1; continue ;; esac
    filtered+=("${args[$i]}")
done

json_escape(){ s="$1"; s="${s//\\/\\\\}"; s="${s//\"/\\\"}"; s="${s//$'\n'/}"; printf "%s" "$s"; }
if (( has_c==1 )) && [[ -n "$src" ]]; then
    args_json="\"$(json_escape "$HOST_CXX")\""
    # 主机编译器参数顺序：flags + -c + src
    for a in "${host_flags[@]}"; do args_json+=",\"$(json_escape "$a")\""; done
    args_json+=",\"-c\""
    args_json+=",\"$(json_escape "$src")\""
    printf '{"directory":"%s","arguments":[%s],"file":"%s"}\n' "$(pwd)" "$args_json" "$src" >> "$out"
fi
exec "$REAL_NVCC" "${filtered[@]}"
EOS

                        chmod +x "${WRAPDIR}/gcc" "${WRAPDIR}/g++" "${WRAPDIR}/nvcc"
            export PATH="${WRAPDIR}:$PATH"

                        # mpicxx 包装器：很多 AMReX + CUDA 构建实际通过 mpicxx 作为 nvcc 驱动，这里沿用 nvcc 记录逻辑
                        cat > "${WRAPDIR}/mpicxx" <<'EOS'
#!/usr/bin/env bash
out=.nvcc_commands.jsonl
args=("$@")

has_c=0; src=""
for a in "${args[@]}"; do
    [[ "$a" == "-c" ]] && has_c=1
    case "$a" in *.c|*.cc|*.cpp|*.cxx|*.C|*.cu) src="$a" ;; esac
done

HOST_CXX="${REAL_GXX:-$(command -v g++ || echo g++)}"
for ((i=0;i<${#args[@]};i++)); do
    a="${args[$i]}"
    case "$a" in
        -ccbin=*) HOST_CXX="${a#-ccbin=}" ;;
        -ccbin)
            if (( i+1 < ${#args[@]} )); then HOST_CXX="${args[$((i+1))]}"; fi ;;
    esac
done

host_flags=()
have_std=0
for ((i=0;i<${#args[@]};i++)); do
    a="${args[$i]}"
    case "$a" in
        -I*|-isystem*|-D*|-O*|-g|-f*) host_flags+=("$a") ;;
        -I|-isystem)
            if (( i+1 < ${#args[@]} )); then
                nxt="${args[$((i+1))]}"
                if [[ -n "$nxt" && ! "$nxt" =~ ^- ]]; then
                    host_flags+=("$a" "$nxt")
                    ((i++))
                    continue
                fi
            fi
            ;;
        -m32|-m64|-march=*|-mtune=*|-mcpu=*|-mfpu=*|-mno-*|-mavx*|-msse*|-mfma*) host_flags+=("$a") ;;
        -maxrregcount|-maxrregcount=*) ;;
        -std=*) have_std=1; host_flags+=("$a") ;;
        -Xcompiler)
            if (( i+1 < ${#args[@]} )); then
                IFS=',' read -r -a xc <<< "${args[$((i+1))]}"
                for x in "${xc[@]}"; do [[ -n "$x" ]] && host_flags+=("$x"); done
            fi ;;
        -Xcompiler=*)
            IFS=',' read -r -a xc <<< "${a#-Xcompiler=}"
            for x in "${xc[@]}"; do [[ -n "$x" ]] && host_flags+=("$x"); done ;;
    esac
done
[[ $have_std -eq 0 ]] && host_flags+=("-std=c++17")

filtered=()
skip_next=0
for ((i=0;i<${#args[@]};i++)); do
    if (( skip_next )); then skip_next=0; continue; fi
    case "${args[$i]}" in -MMD|-MP) continue ;; -MF) skip_next=1; continue ;; esac
    filtered+=("${args[$i]}")
done

json_escape(){ s="$1"; s="${s//\\/\\\\}"; s="${s//\"/\\\"}"; s="${s//$'\n'/}"; printf "%s" "$s"; }
if (( has_c==1 )) && [[ -n "$src" ]]; then
    args_json="\"$(json_escape "$HOST_CXX")\""
    for a in "${host_flags[@]}"; do args_json+=",\"$(json_escape "$a")\""; done
    args_json+=",\"-c\""
    args_json+=",\"$(json_escape "$src")\""
    printf '{"directory":"%s","arguments":[%s],"file":"%s"}\n' "$(pwd)" "$args_json" "$src" >> "$out"
fi
exec "$REAL_MPICXX" "${filtered[@]}"
EOS

                        # mpicc 包装器：仅透传，便于未来捕获 C 代码（当前项目基本为 C++，此处与 mpicxx 一致处理）
                        cat > "${WRAPDIR}/mpicc" <<'EOS'
#!/usr/bin/env bash
exec "$REAL_MPICC" "$@"
EOS
                        chmod +x "${WRAPDIR}/mpicxx" "${WRAPDIR}/mpicc"

                        # git 包装器：在非 git 仓库下静默返回空结果，避免 "fatal: not a git repository" 噪声
                        cat > "${WRAPDIR}/git" <<'EOS'
#!/usr/bin/env bash
REAL_GIT_BIN="${REAL_GIT:-$(command -v git || echo /usr/bin/git)}"
subcmd="$1"
case "$subcmd" in
    describe|rev-parse)
        # 静默 stderr，若失败则返回空串并退出 0，避免 make 输出 fatal 噪声
        out="$($REAL_GIT_BIN "$@" 2>/dev/null || true)"
        if [ -n "$out" ]; then
            printf "%s\n" "$out"
            exit 0
        else
            exit 0
        fi
        ;;
    *)
        exec "$REAL_GIT_BIN" "$@"
        ;;
esac
EOS
                        chmod +x "${WRAPDIR}/git"

            "${make_cmd[@]}" || make_status=$?
            if [[ ! -s .cc_commands.jsonl && ! -s .cxx_commands.jsonl ]]; then
                info "未捕获到编译步骤，强制重建以生成编译数据库..."
                make -B -j"${MAKE_J}" CUDA_ARCH="${CUDA_ARCH}" || make_status=$?
            fi
            echo '[' > compile_commands.json
            local first=1
            for f in .cc_commands.jsonl .cxx_commands.jsonl .nvcc_commands.jsonl; do
                if [[ -f "$f" ]]; then
                    while IFS= read -r line; do
                        if [[ $first -eq 1 ]]; then first=0; else echo ',' >> compile_commands.json; fi
                        echo "$line" >> compile_commands.json
                    done < "$f"
                fi
            done
            echo ']' >> compile_commands.json
        python3 - <<'PY' 2>/dev/null || true
import re, json, shlex
p = 'compile_commands.json'
try:
        txt = open(p,'r',encoding='utf-8').read()
except Exception:
        raise SystemExit(0)
changed = False
def repl(m):
        global changed
        s = m.group(1)
        try:
                tokens = shlex.split(s)
        except Exception:
                tokens = []
        changed = True
        return '"arguments":' + json.dumps(tokens, ensure_ascii=False) + ',"file"'
new = re.sub(r'"command":"(.*?)","file"', repl, txt, flags=re.DOTALL)
if changed:
        open(p,'w',encoding='utf-8').write(new)
PY

            # 进一步规范化：拆分粘连 token、移除 nvcc 专属参数与无效 -isystem
            python3 - <<'PY' 2>/dev/null || true
import json, re, os
p='compile_commands.json'
try:
    db=json.load(open(p,'r',encoding='utf-8'))
except Exception:
    raise SystemExit(0)

def split_tokens(args):
    out=[]
    for t in args:
        # 如果某个元素意外包含多个 flag（被上游打成单个参数），按空格拆分
        parts=str(t).split()
        out.extend(parts)
    return out

def clean_tokens(args):
    out=[]; i=0; n=len(args)
    while i<n:
        t=args[i]
        # 过滤 nvcc 专属/设备端参数
        if (t.startswith('-Xptxas') or t.startswith('-Xcudafe') or t.startswith('--expt-') or
            t=='--forward-unknown-to-host-compiler' or t.startswith('--diag_suppress')):
            i+=1; continue
        if t.startswith('-gencode') or t.startswith('-arch') or t.startswith('--device-'):
            i+=1; continue
        if t.startswith('-D__CUDA_ARCH__') or t.startswith('-D__CUDA_ARCH_LIST__'):
            i+=1; continue
        # 处理 -maxrregcount[=N] 或成对出现的 "-maxrregcount N"
        if t=='-maxrregcount':
            i+=2; continue
        if t.startswith('-maxrregcount='):
            i+=1; continue
        # 清理孤立的 -isystem（后面没有路径或下一个是另一个 flag）
        if t=='-isystem':
            nxt=args[i+1] if i+1<n else None
            if (nxt is None) or (str(nxt).startswith('-')):
                i+=1; continue
            else:
                out.append(t); out.append(nxt); i+=2; continue
        out.append(t); i+=1
    return out

changed=False
for e in db:
    args=e.get('arguments')
    if not isinstance(args,list):
        continue
    a1=split_tokens(args)
    a2=clean_tokens(a1)
    if a2!=args:
        e['arguments']=a2
        changed=True

# 过滤掉 nvcc 生成的临时 cudafe/tmpxft 记录（CCDB_KEEP_TMP=1 时跳过过滤）
keep_tmp = os.environ.get('CCDB_KEEP_TMP','0') == '1'
if not keep_tmp:
    def is_tmpxft(entry):
        f=str(entry.get('file',''))
        return f.startswith('/tmp/tmpxft_') or ('cudafe' in f)
    new_db=[e for e in db if not is_tmpxft(e)]
    if len(new_db)!=len(db):
        db=new_db
        changed=True

dedup = os.environ.get('CCDB_DEDUP_BY_FILE','1') == '1'
if dedup:
    # 按文件去重（保留最后一个记录）
    by_file={}
    for e in db:
        f=e.get('file')
        if f:
            by_file[f]=e
    db=list(by_file.values())

if changed:
    json.dump(db, open(p,'w',encoding='utf-8'), ensure_ascii=False, indent=2)
PY
    fi
    else
    info "GEN_CCDB=0，跳过生成编译数据库"
    "${make_cmd[@]}" || make_status=$?
    fi

    return "${make_status}"
}

# 2.5) 成功后的构建产物提示（不改变返回码，仅友好提示）
verify_artifacts() {
    # 常见可执行文件命名：main*.ex 或 *.ex；也检测是否生成了目标文件目录
    local found=0
    shopt -s nullglob
    for f in main*.ex *.ex; do
        if [[ -x "$f" ]]; then
            info "检测到可执行文件: $f"
            found=1
            break
        fi
    done
    shopt -u nullglob
    if [[ $found -eq 0 ]]; then
        if compgen -G "tmp_build_dir/o/*" >/dev/null 2>&1; then
            info "未发现 .ex 可执行，但检测到目标文件已生成（tmp_build_dir/o/）。可能是只编译了部分目标或增量构建。"
        else
            info "make 返回 0，但未检测到可执行或目标文件。请确认默认目标是否生成可执行，或是否指定了其他目标。"
        fi
    fi
}

# 3) 在当前主机执行完整流程
run_here() {
    info "加载编译环境..."
    load_environment
    info "项目根目录: ${PROJECT_ROOT}"
    info "环境已加载，开始编译 (J=${MAKE_J}, CUDA_ARCH=${CUDA_ARCH})..."
    if do_compile; then
        info "编译成功！"
        verify_artifacts
    else
        rc=$?
        echo "编译失败，请检查错误信息。" >&2
        return "$rc"
    fi
}

# 4) 判定本地/远程（固定目标：whshare-agent-1）
if [[ "$(hostname)" == "whshare-agent-1" ]]; then
    info "已在编译节点 whshare-agent-1 上，直接编译..."
    run_here
    exit $?
else
    info "正在连接到编译节点 whshare-agent-1..."
    # 将本脚本通过标准输入传到远端执行，并传递必要的环境变量
    if [[ -z "$SCRIPT_PAYLOAD" || ! -f "$SCRIPT_PAYLOAD" ]]; then
        echo "错误: 无法找到脚本文件 $SCRIPT_PAYLOAD" >&2
        exit 1
    fi
    ssh_workdir=$(printf '%q' "$PROJECT_ROOT")
    ssh_make_j=$(printf '%q' "$MAKE_J")
    ssh_cuda_arch=$(printf '%q' "$CUDA_ARCH")
    ssh_gen_ccdb=$(printf '%q' "$GEN_CCDB")
    ssh_ccdb_method=$(printf '%q' "$CCDB_METHOD")
    ssh_make_keep=$(printf '%q' "$MAKE_KEEP_GOING")
    ssh_project_root=$(printf '%q' "$PROJECT_ROOT")
    ssh whshare-agent-1 "cd ${ssh_workdir}; \
        MAKE_J=${ssh_make_j} \
        CUDA_ARCH=${ssh_cuda_arch} \
        GEN_CCDB=${ssh_gen_ccdb} \
        CCDB_METHOD=${ssh_ccdb_method} \
        MAKE_KEEP_GOING=${ssh_make_keep} \
        _CHANNELFLOW_PROJECT_ROOT=${ssh_project_root} \
        bash -s" < "${SCRIPT_PAYLOAD}"
    rc=$?
    if [ $rc -eq 0 ]; then
        info "远程编译成功。"
    else
        echo "远程编译失败 (exit $rc)。" >&2
    fi
    exit $rc
fi