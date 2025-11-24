# cpptools 配置说明

本文件夹用于存放所有与 cpptools 相关的 VSCode 配置文件。

## 目录结构与文件说明
- `.vscode/c_cpp_properties.json`：cpptools 主配置文件，需放置于 `.vscode/` 目录。
- `.vscode/settings.json`：推荐的 VSCode 设置，需放置于 `.vscode/` 目录。
- `compile_commands.json`：编译数据库，需放置于项目根目录。

## 切换流程
1. 执行 `../switch_config.sh cpptools`。
2. 脚本会自动删除原有配置并复制本文件夹下所有配置到正确位置。
3. 切换完成后，VSCode 会自动识别 cpptools 配置。

## 注意事项
- 切换时会覆盖原有相关配置文件，请提前备份重要内容。
