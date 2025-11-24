# clangd 配置说明

本文件夹用于存放所有与 clangd 相关的 VSCode 配置文件。

## 目录结构与文件说明
- `.clangd`：clangd 主配置文件，需放置于项目根目录。
- `.vscode/settings.json`：推荐的 VSCode 设置，需放置于 `.vscode/` 目录。
- `compile_commands.json`：编译数据库，需放置于项目根目录。
 - `learnamerx.code-workspace`：clangd 方案工作区文件；切换脚本会复制到根目录覆盖。

### `.clangd` 关键段说明
```
CompileFlags:
	Add: [-std=c++17, -Wall, -Wextra]
Index:
	Background: Build        # 持续后台索引全部工程
Diagnostics:
	UnusedIncludes: Strict   # 提示未使用的 include
Completion:
	AllScopes: true          # 完成结果包含所有可见作用域
	IncludeInsertion: None   # 不自动插入头文件
```

## 切换流程
1. 执行 `../switch_config.sh clangd`。
2. 脚本会自动删除原有配置并复制本文件夹下所有配置到正确位置。
3. 切换完成后，VSCode 会自动识别 clangd 配置。

## 注意事项
- 切换时会覆盖原有相关配置文件，请提前备份重要内容。
 - 若出现跳转迟缓，可手动执行：命令面板 > "Clangd: Restart language server"。
 - 若 compile_commands.json 路径改变，请更新 `.clangd` 或工作区设置中的 `clangd.compileCommandsPath`。
