# 元素周期表 Pro — Linux / 统信 UOS 自包含版

专业完整的交互式元素周期表，Qt 6 / C++17 开发，原生 Linux 版本。

## 特点：真正的自包含

**无需在目标系统安装 Qt 或中文字体**。所有运行依赖（Qt6 动态库、X11 插件、中文字体）都已打包在内，解压即用。`run.sh` 会自动设置 Qt 库路径、插件路径和私有 fontconfig，中文使用自带的 Noto Sans SC 字体，覆盖全部 118 个元素中文名（含超重元素的扩展区生僻字 𬬻 𬭊 𬭳 𬭛 𬭶 𫟼 𬬭 鿔 鿭 𫓧 鿬 鿫 等）。

## 系统要求

- x86-64 架构的 Linux 发行版（统信 UOS、深度 Deepin、Ubuntu、麒麟、Debian 等）
- 无需预装任何 Qt 运行库；二进制最高要求 GLIBC_2.34（Ubuntu 22.04+ / Debian 12+ 等均可运行）

## 快速开始（免安装，推荐）

```bash
tar -xzf periodic-table-linux-selfcontained.tar.gz
cd periodic-table-linux-selfcontained
chmod +x run.sh PeriodicTable
./run.sh
```

本目录中的 `run.sh` 会自动检测依赖：若本地已有 `lib/`/`fonts/` 则直接使用，否则从 `periodic-table-linux-selfcontained.tar.gz` 临时解压运行；两种入口都使用完整的自包含配置。

## 系统安装（可选）

```bash
sudo chmod +x install.sh
sudo ./install.sh
```

安装内容：
- 程序与全部依赖 → `/usr/local/lib/periodic-table`
- 启动器 → `/usr/local/bin/PeriodicTable`（自动带库路径和字体配置）
- 桌面入口 + 图标 → 开始菜单可见"元素周期表"

安装后命令行直接运行 `PeriodicTable`，或开始菜单搜索启动。
卸载：`sudo ./uninstall.sh`

## 无图形桌面时的测试

```bash
QT_QPA_PLATFORM=offscreen ./run.sh --size 1280x720 --screenshot test.png
```

## 功能

- 118 个元素完整数据，含中文名/英文名/原子量/电子构型/电负性/周期族/区块等
- 分类多选筛选、关键词搜索（符号/名称/原子序数）
- 玻璃拟态现代化界面、自定义标题栏、丝滑缩放
- 高分屏（HiDPI）自适应

## 文件说明

| 文件 | 用途 |
|------|------|
| `periodic-table-linux-selfcontained.tar.gz` | 完整自包含包（约 35 MB，含 Qt6 库/插件/中文字体），推荐分发该文件 |
| `periodic-table-linux-x64.tar.gz` | 仅主程序的小包（需目标系统自带 Qt6，不推荐单独分发） |
| `PeriodicTable` | 主程序（ELF x86-64） |
| `run-selfcontained.sh` | 本地源码目录直接运行用（设置自包含库路径/字体） |
| `install.sh` / `uninstall.sh` | 系统安装 / 卸载 |
| `periodic-table.desktop` | 桌面入口 |

## 验证记录

- WSL Kali Linux 2026.3（x86-64）中解压自包含包，使用包内 `run.sh` 以 offscreen 模式分别以 800×600 与 1280×720 渲染成功；中文元素名显示正常，118 个元素与详情面板布局完整。
- 使用 `LD_LIBRARY_PATH=lib` 检查 `ldd PeriodicTable`，所有依赖均可解析。
- 原 README 宣称的"纯净 Ubuntu 26.04 验证"与字体大小描述已按实际打包内容更正。
- 修复说明：自包含 `run.sh` 现把 fontconfig 配置与缓存写入用户私有缓存目录（`$XDG_CACHE_HOME` 或 `~/.cache`），避免 `sudo install.sh` 安装后普通用户运行失败；本地 `run.sh` 改为自动检测并从 tarball 解压运行。
- 包校验值见同目录 `SHA256SUMS.txt`。
## 更新记录

- 2026-11-23：修复详情卡片中英文名/中文名在窄面板被裁切的问题（改用 QFontMetrics::elidedText 并重新平衡布局）；Windows 与 Linux 两个自包含包均已用修复后的二进制重建。Linux 包 SHA256 已更新至 SHA256SUMS.txt。
