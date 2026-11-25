#!/usr/bin/env bash
# 元素周期表 Pro — 用户级安装（无需 root 权限）
# 用法: ./install-user.sh   —— 安装到当前用户 ~/.local，不触碰系统目录
set -e
APPDIR="$(cd "$(dirname "$0")" && pwd)"

DATA_HOME="${XDG_DATA_HOME:-$HOME/.local/share}"
INSTALLDIR="${XDG_DATA_HOME:-$HOME/.local}/periodic-table"
BINDIR="$HOME/.local/bin"                 # 可执行文件放 ~/.local/bin
DESKTOPDIR="$DATA_HOME/applications"      # ~/.local/share/applications
ICONDIR="$DATA_HOME/icons/hicolor/256x256/apps"

echo "==> 用户级安装元素周期表 Pro"
echo "    安装目录: $INSTALLDIR"
echo "    启动器:   $BINDIR/PeriodicTable"
echo "    （无需 root 权限，全部写入当前用户目录）"

mkdir -p "$INSTALLDIR" "$BINDIR" "$DESKTOPDIR" "$ICONDIR"

# 整个自包含目录（程序 + Qt6 库 + 插件 + 字体）装到用户目录
cp -a "$APPDIR"/. "$INSTALLDIR/"
chmod +x "$INSTALLDIR/PeriodicTable" "$INSTALLDIR/run.sh" 2>/dev/null || true

# 启动包装脚本：设置库路径、插件路径、私有字体配置
cat > "$BINDIR/PeriodicTable" <<EOF
#!/usr/bin/env bash
DIR="$INSTALLDIR"
export LD_LIBRARY_PATH="\$DIR/lib\${LD_LIBRARY_PATH:+:\$LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="\$DIR/plugins\${QT_PLUGIN_PATH:+:\$QT_PLUGIN_PATH}"
export QT_QPA_PLATFORM_PLUGIN_PATH="\$DIR/plugins/platforms"
# 私有 fontconfig：使用包内 Noto Sans CJK SC，缓存写用户目录
CACHE_BASE="\${XDG_CACHE_HOME:-\$HOME/.cache}/periodic-table"
mkdir -p "\$CACHE_BASE" 2>/dev/null || true
CONF="\$CACHE_BASE/fonts.conf"
cat > "\$CONF" <<FCEOF
<?xml version="1.0"?>
<!DOCTYPE fontconfig SYSTEM "fonts.dtd">
<fontconfig>
  <dir>\$DIR/fonts</dir>
  <cachedir>\$CACHE_BASE/fontcache</cachedir>
  <match target="pattern">
    <test name="lang" compare="contains"><string>zh</string></test>
    <edit name="family" mode="prepend" binding="strong"><string>Noto Sans CJK SC</string></edit>
  </match>
  <match target="pattern">
    <test qual="any" name="family"><string>sans-serif</string></test>
    <edit name="family" mode="prepend" binding="strong"><string>Noto Sans CJK SC</string></edit>
  </match>
</fontconfig>
FCEOF
export FONTCONFIG_FILE="\$CONF"
exec "\$DIR/PeriodicTable" "\$@"
EOF
chmod +x "$BINDIR/PeriodicTable"

# 图标
[ -f "$INSTALLDIR/periodic-table.png" ] && \
    cp -f "$INSTALLDIR/periodic-table.png" "$ICONDIR/periodic-table.png"

# 桌面入口（Exec 指向用户级启动器）
sed "s|^Exec=.*|Exec=$BINDIR/PeriodicTable|" "$INSTALLDIR/periodic-table.desktop" \
    > "$DESKTOPDIR/periodic-table.desktop" 2>/dev/null || true
chmod +x "$DESKTOPDIR/periodic-table.desktop" 2>/dev/null || true

# 刷新桌面数据库（若可用，无需 root）
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database "$DESKTOPDIR" >/dev/null 2>&1 || true
fi

echo "==> 安装完成"
echo "    命令行启动: $BINDIR/PeriodicTable"
echo "    或开始菜单搜索『元素周期表』"
if [[ ":$PATH:" != *":$BINDIR:"* ]]; then
  echo "    提示: 若命令行找不到 PeriodicTable，请将 $BINDIR 加入 PATH"
fi
