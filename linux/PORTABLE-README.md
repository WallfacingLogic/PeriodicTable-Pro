# 元素周期表 Pro — Linux 便携版 / 用户级安装

## 无需 root 权限

本包为**完全自包含**版本：内置 Qt 6 库、X11 插件、中文字体。所有安装方式都**不需要 root 权限**。

## 方式一：解压即用（最简单）

```bash
tar -xzf PeriodicTable-1.0.2-linux-x64.tar.gz
cd PeriodicTable-1.0.2-linux-x64
./run.sh
```

## 方式二：用户级安装（推荐，开始菜单可见）

```bash
cd PeriodicTable-1.0.2-linux-x64
./install-user.sh
```

- 程序装到 `~/.local/periodic-table`
- 启动器装到 `~/.local/bin/PeriodicTable`
- 桌面入口装到 `~/.local/share/applications`（开始菜单可见"元素周期表"）
- 全部写入当前用户目录，**不需要 sudo**

卸载：

```bash
./uninstall-user.sh
```

命令行找不到 `PeriodicTable` 时，把 `~/.local/bin` 加入 PATH：

```bash
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

## 系统要求

- x86-64 Linux，glibc ≥ 2.28
- 适配统信 UOS、深度 Deepin、麒麟、Ubuntu 20.04+、Debian 10+ 等
- 无需预装 Qt 或中文字体

## 无图形桌面测试

```bash
QT_QPA_PLATFORM=offscreen ./run.sh --size 1280x720 --screenshot test.png
```
