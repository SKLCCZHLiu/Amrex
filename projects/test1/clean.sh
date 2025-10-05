#!/usr/bin/env bash
# 容错清理：在无匹配文件时不要报错退出
shopt -s nullglob dotglob 2>/dev/null || true
rm -rf Backtrace* *.log *.dat cp* case* .c* .w* tmp_build_dir 2>/dev/null || true
exit 0
