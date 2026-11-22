#!/usr/bin/env bash
# 元素周期表 Pro — 自包含版系统安装脚本
# 无需系统预装 Qt / 中文字体，所有依赖已随包携带
# 用法: sudo ./install.sh
set -e
APPDIR="$(cd "$(dirname "$0")" && pwd)"
PREFIX="${PREFIX:-/usr/local}"
INSTALLDIR="$PREFIX/lib/periodic-table"
BINDIR="$PREFIX/bin"
DESKTOPDIR="$PREFIX/share/applications"
ICONDIR="$PREFIX/share/icons/hicolor/256x256/apps"

echo "==> 安装元素周期表 Pro (自包含版) 到 $PREFIX ..."
install -d "$INSTALLDIR" "$BINDIR" "$DESKTOPDIR" "$ICONDIR"

# 整个自包含目录装到 lib/periodic-table
cp -a "$APPDIR"/. "$INSTALLDIR/"
chmod +x "$INSTALLDIR/PeriodicTable" "$INSTALLDIR/run.sh"

# 创建启动包装脚本（设置 LD_LIBRARY_PATH / 字体）
cat > "$BINDIR/PeriodicTable" <<EOF
#!/usr/bin/env bash
exec "$INSTALLDIR/run.sh" "\$@"
EOF
chmod +x "$BINDIR/PeriodicTable"

# 图标
[ -f "$INSTALLDIR/periodic-table.png" ] && \
    install -m 0644 "$INSTALLDIR/periodic-table.png" "$ICONDIR/periodic-table.png"

# 桌面入口
sed "s|^Exec=.*|Exec=$BINDIR/PeriodicTable|" "$INSTALLDIR/periodic-table.desktop" \
    > "$DESKTOPDIR/periodic-table.desktop" 2>/dev/null || true

if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database "$DESKTOPDIR" || true
fi
echo "==> 安装完成。开始菜单搜索『元素周期表』，或命令行运行: PeriodicTable"
