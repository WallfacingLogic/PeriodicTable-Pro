<div align="center">

# 元素周期表 Pro

**Periodic Table Pro** — 专业、完整、现代化的交互式元素周期表

C++17 · Qt 6 · 单文件零依赖 · Windows / Linux 双平台

</div>

---

## 简介

一款用 C++17 + Qt 6 Widgets 从零手写的桌面元素周期表。周期表网格、玻璃拟态背景、标题栏全部自绘，不依赖 QTableWidget 等控件，追求"专业数据 + 现代视觉 + 丝滑交互"。

## 功能

- **118 个元素完整数据**：中文名、英文名、原子量、周期/族、区块（s/p/d/f）、电子构型、电负性、密度、熔点、沸点、常温状态、价电子数、发现年份
- **分类多选筛选**：碱金属、碱土金属、过渡金属、后过渡金属、类金属、非金属、卤素、稀有气体、镧系、锕系，任意组合
- **即时搜索**：按符号 / 中英文名 / 原子序数，120 ms 防抖
- **玻璃拟态界面**：渐变光晕背景、圆角卡片、悬停信息浮卡、自绘无边框标题栏
- **丝滑缩放**：拖动窗口时用整帧纹理拉伸保持玻璃质感，停止后 180 ms 精绘恢复锐利
- **HiDPI 自适应**：1.0 / 1.25 / 1.5 / 2.0 缩放比例下均正确渲染
- **超重元素中文名**：内置字体子集覆盖扩展区生僻字（𬬻 𬭊 𬭳 𬭛 𬭶 𫟼 𬬭 鿔 鿭 𫓧 鿬 鿫 等）

## 界面

> 运行程序后可见完整周期表。截图请见 [Releases](../../releases) 页面。

## 下载

前往 [Releases](../../releases) 获取最新版本：

| 平台 | 文件 | 说明 |
|------|------|------|
| Windows x64 | `PeriodicTable-<ver>-win64.7z` | 自包含，解压即用，无需安装 Qt |
| Linux x64 (APT) | `periodictable-pro_<ver>_amd64.deb` | **推荐**，Debian/Ubuntu/统信/麒麟可直接 apt 安装 |
| Linux x64 | `PeriodicTable-<ver>-linux-x64.tar.gz` | 自包含，含 Qt6 库与中文字体，解压即用 |

### Windows

```powershell
# 解压后双击 PeriodicTable.exe
```

### Linux — APT 安装（推荐，Debian / Ubuntu / 统信 UOS / 麒麟）

```bash
sudo apt install ./periodictable-pro_*-amd64.deb
# 命令行运行：
PeriodicTable
# 或在开始菜单搜索“元素周期表”
# 卸载：
sudo apt remove --purge periodictable-pro
```

`.deb` 已内含全部 Qt6 库、插件与中文字体，无需额外安装依赖（仅依赖系统 glibc/libstdc++）。

### Linux — 免安装解压版

```bash
tar -xzf PeriodicTable-*-linux-x64.tar.gz
cd PeriodicTable-*-linux-x64
./run.sh              # 免安装运行
sudo ./install.sh     # 或安装到系统（开始菜单可见）
```

## 从源码构建

### 依赖

- CMake ≥ 3.16
- C++17 编译器（MSVC 2022 / GCC 11+ / Clang 14+）
- Qt 6.2+（Core、Gui、Widgets）

### Windows

```powershell
cmake -S . -B build -G "Visual Studio 18 2026" -A x64 `
      -DCMAKE_PREFIX_PATH="C:\Qt\6.12.0\msvc2022_64"
cmake --build build --config Release
# 产物: build\Release\PeriodicTable.exe
```

### Linux

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
# 产物: build/PeriodicTable
```

## 项目结构

```
├── CMakeLists.txt            # 跨平台构建配置
├── main.cpp                  # 入口、Fusion 样式、图标
├── mainwindow.{h,cpp}        # 主窗口、搜索、分类筛选、DWM/无边框
├── periodictablewidget.{h,cpp}  # 自绘周期表、命中网格、双路径渲染缓存
├── elementdetailpanel.{h,cpp}   # 详情面板、Hero 卡片
├── elementdata.{h,cpp}       # 118 元素数据与检索
├── glassstyle.{h,cpp}        # 玻璃主题 QSS 与背景绘制
├── titlebar.{h,cpp}          # 自绘标题栏与窗口按钮
├── resources.qrc             # 图标资源
└── linux/                    # Linux 打包脚本与说明
```

## 设计要点

- **O(1) 命中测试**：18×10 网格索引，鼠标移动不遍历元素
- **静态缓存 + 脏矩形**：背景与表格各缓存为 1:1 位图，悬停只重绘脏区，避免抗锯齿残影
- **双路径渲染**：缩放中整帧拉伸（保留质感），停止后精绘（恢复锐利）
- **DPR 安全**：缓存位图一律 `drawPixmap(0,0,pix)` 贴图，规避高 DPI 下的坐标空间混淆

## 贡献

欢迎 Issue 与 PR。提交前请确保两个平台均可构建通过。

## 许可证

[MIT](LICENSE)

## 致谢

- 元素数据参考 IUPAC 及公开化学资料
- 内置中文字体：Noto Sans CJK SC（SIL Open Font License 1.1）子集
