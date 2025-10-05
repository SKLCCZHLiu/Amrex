# ChannelFlow

## 概览
3D channel flow with immersed particles (AMR LBM)。

## 目录结构
- `src/` — 原始 C++/CUDA 源码，直接从学习项目复制。
- `config/` — 构建配置与 AMReX 输入文件。
- `scripts/` — 编译、清理与提交脚本。
- `data/` — 所有模拟输出数据文件（粒子数据、速度剖面、绘图文件等）。
- `docs/` — 附加文档（如有）。

**注意**：所有模拟输出文件现在都会写入 `data/` 目录。详见 `DATA_OUTPUT_CHANGES.md`。

## 编译
在项目根目录执行脚本即可完成远程编译，并自动解析目录结构：

```bash
./scripts/compile.sh
```

如需调整并行度、CUDA 架构或生成 `compile_commands.json`，可通过环境变量（`MAKE_J`、`CUDA_ARCH`、`GEN_CCDB` 等）覆盖默认配置。

## 同步说明
这些文件由 `tools/sync_projects.py` 自动同步。如需再次更新，请在仓库根目录运行：

```bash
python tools/sync_projects.py --project ChannelFlow
```

加入 `--clean` 可先清空目标目录，`--extra-exclude PATTERN` 可临时排除更多文件。
