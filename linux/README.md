# 元素周期表 Pro — Linux / 统信 UOS 自包含版

专业完整的交互式元素周期表，Qt 6 / C++17 开发，原生 Linux 版本。

## 特点：真正的自包含

**无需在目标系统安装 Qt 或中文字体**。所有运行依赖（Qt6 动态库、X11 插件、中文字体）都已打包在内，解压即用。`run.sh` 会自动设置 Qt 库路径、插件路径和私有 fontconfig，中文使用自带的 Noto Sans SC 字体，覆盖全部 118 个元素中文名（含超重元素的扩展区生僻字 𬬻 𬭊 𬭳 𬭛 𬭶 𫟼 𬬭 鿔 鿭 𫓧 鿬 鿫 等）。

## 系统要求

- x86-64 架构的 Linux 发行版（统信 UOS、深度 Deepin、Ubuntu、麒麟、Debian 等）
- 无需预装任何 Qt 运行库；基于 Qt 6.2.4 + glibc 2.28 构建，**兼容统信 UOS（glibc 2.28）、Debian 10+、Ubuntu 20.04+**
- **所有安装方式都不需要 root 权限**

## 用户级安装（无 root 权限，推荐）

没有 root 权限、或图形包管理器提示"库中没有依赖项"时，用用户级安装：

```bash
tar -xzf PeriodicTable-*-linux-x64.tar.gz
cd PeriodicTable-*-linux-x64
./install-user.sh     # 装到 ~/.local，无需 sudo
# ./uninstall-user.sh # 卸载
```

程序装到 `~/.local/periodic-table`，启动器 `~/.local/bin/PeriodicTable`，桌面入口 `~/.local/share/applications`（开始菜单可见）。命令行找不到时把 `~/.local/bin` 加入 PATH。

## APT 安装（有 root 权限时，统信 UOS / Deepin / Ubuntu / 麒麟）

在 Release 页面下载 `periodictable-pro_<ver>_amd64.deb`（需 root）：

```bash
sudo apt install ./periodictable-pro_*-amd64.deb
PeriodicTable              # 命令行启动，或开始菜单搜索"元素周期表"
sudo apt remove --purge periodictable-pro   # 卸载
```

`Depends` 已修正为 `libc6 (>= 2.28)`，统信 UOS 的图形包管理器不会再报"库中没有依赖项"。已内含全部 Qt6 库与中文字体。无 root 权限时用上面的"用户级安装"。

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
| `periodictable-pro_<ver>_amd64.deb` | Debian/Ubuntu/统信/麒麟系统包，`apt install` 即用（需 root） |
| `PeriodicTable-<ver>-linux-x64.tar.gz` | **完整自包含便携包（约 30 MB）**，含 Qt6 库/插件/中文字体，解压即用 |
| `install-user.sh` / `uninstall-user.sh` | **用户级安装 / 卸载（无需 root）** |
| `run.sh` | 自包含运行脚本（设置库路径/插件路径/私有字体） |
| `PeriodicTable` | 主程序（ELF x86-64，仅要求 GLIBC_2.14） |
| `periodic-table.desktop` / `periodic-table.png` | 桌面入口 / 图标 |

## 验证记录

- WSL Kali Linux 2026.3（x86-64）中解压自包含包，使用包内 `run.sh` 以 offscreen 模式分别以 800×600 与 1280×720 渲染成功；中文元素名显示正常，118 个元素与详情面板布局完整。
- 使用 `LD_LIBRARY_PATH=lib` 检查 `ldd PeriodicTable`，所有依赖均可解析。
- 原 README 宣称的"纯净 Ubuntu 26.04 验证"与字体大小描述已按实际打包内容更正。
- 修复说明：自包含 `run.sh` 现把 fontconfig 配置与缓存写入用户私有缓存目录（`$XDG_CACHE_HOME` 或 `~/.cache`），避免 `sudo install.sh` 安装后普通用户运行失败；本地 `run.sh` 改为自动检测并从 tarball 解压运行。
- 包校验值见同目录 `SHA256SUMS.txt`。
## 更新记录

- 2026-11-23：修复详情卡片中英文名/中文名在窄面板被裁切的问题（改用 QFontMetrics::elidedText 并重新平衡布局）；Windows 与 Linux 两个自包含包均已用修复后的二进制重建。Linux 包 SHA256 已更新至 SHA256SUMS.txt。
- 2026-11-23：新增 Debian/Ubuntu `.deb` 系统包（`periodictable-pro_1.0.0_amd64.deb`）。在纯净 Ubuntu 26.04 中用 `apt install ./periodictable-pro_1.0.0_amd64.deb` 实测：依赖解析通过、安装成功、开始菜单入口与图标就位、`PeriodicTable` 命令行可运行、中文与超重元素显示正常、连续 3 次运行输出一致、`apt remove --purge` 卸载无残留。构建脚本见 `scripts/build-deb.sh`。
