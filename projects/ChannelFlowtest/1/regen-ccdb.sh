#!/usr/bin/env bash
set -euo pipefail
export GEN_CCDB=1
export MAKE_KEEP_GOING=1
MAKE_J=${MAKE_J:-8}
CUDA_ARCH=${CUDA_ARCH:-80}
if [[ -x ./compile.sh ]]; then
  MAKE_J="$MAKE_J" CUDA_ARCH="$CUDA_ARCH" GEN_CCDB=1 ./compile.sh || true
else
  echo "compile.sh missing; falling back to plain make capture" >&2
  if command -v bear >/dev/null 2>&1; then
    bear -- make -j"$MAKE_J" CUDA_ARCH="$CUDA_ARCH" || true
  else
    make -B -j"$MAKE_J" CUDA_ARCH="$CUDA_ARCH" || true
  fi
fi
if [[ -s compile_commands.json ]]; then
  echo "compile_commands.json generated." >&2
else
  echo "Warning: compile_commands.json not generated; clangd may show errors." >&2
fi