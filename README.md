# Chromium Context Menu Mod

一个轻量级的 Chromium 内核浏览器右键菜单管理工具。通过 Hook `SimpleMenuModel::AddItem`，支持根据配置文件自定义屏蔽浏览器右键菜单中的指定选项，还你一个清爽简洁的右键菜单。

## ✨ 功能特性

- **按需屏蔽**：通过菜单项的 `command_id` 精准隐藏不需要的右键菜单项。
- **灵活配置**：使用简单的 `.ini` 配置文件管理黑名单，无需重新编译即可修改规则。
- **调试支持**：内置日志输出开关，方便获取各个菜单项对应的 `command_id` 和名称。
- **兼容性强**：支持自定义目标模块名称（默认 `chrome.dll`）与函数偏移量，适用于 Chromium 内核浏览器。

## 🛠️ 依赖库

- [MinHook](https://github.com/TsudaKageyu/minhook) — 用于实现底层 API Hook。

## 🚀 使用方法

1. **编译项目**
   将源码编译为 `.dll` 文件并重命名为 `version.dll` 放置在浏览器目录中。
2. **创建配置文件**
   在浏览器主程序同级目录下创建 `ccmm.ini` 文件，参考以下模板进行配置：

```ini
[Config]
; 目标模块名称（可选，默认为 chrome.dll）
dll=chrome.dll
; SimpleMenuModel::AddItem 在目标 DLL 中的偏移地址（必填，支持十进制或 0x 开头的十六进制）
offset=0x12345678
; 是否开启调试日志（1 为开启，0 为关闭）
debug=1

[Filter]
; 需要屏蔽的菜单项 command_id 列表（逗号分隔）
blacklist_ids=50100,50101,50102

```

3. **获取菜单项 ID**

- 将 `debug` 设为 `1` 并启动浏览器。
- 使用 `DebugView` 等工具查看日志输出，在浏览器中触发右键菜单时，日志会打印出所有加载的菜单项信息：

```text
[ccmm] [AddItem] command_id: 50100 - 保存为(A)...

```

- 记录下想要隐藏的 `command_id`，填入 `ccmm.ini` 的 `blacklist_ids` 中，重启浏览器即可生效（日常使用建议将 `debug` 改回 `0`）。

## ⚠️ 注意事项

- 浏览器版本更新后，`SimpleMenuModel::AddItem` 的函数偏移地址（`offset`）通常会发生变化，需要重新定位并更新 `ccmm.ini`。
- 插件仅在浏览器主进程（命令行不包含 `--type=`）中初始化，不会影响渲染进程或其他子进程。
