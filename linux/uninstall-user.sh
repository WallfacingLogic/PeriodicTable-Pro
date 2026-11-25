#!/usr/bin/env bash
# 元素周期表 Pro — 用户级卸载（无需 root 权限）
set -e
DATA_HOME="${XDG_DATA_HOME:-$HOME/.local/share}"
BINDIR="$HOME/.local/bin"
DESKTOPDIR="$DATA_HOME/applications"
ICONDIR="$DATA_HOME/icons/hicolor/256x256/apps"

echo "==> 卸载用户级元素周期表 Pro"
rm -f "$BINDIR/PeriodicTable" \
      "$DESKTOPDIR/periodic-table.desktop" \
      "$ICONDIR/periodic-table.png"
rm -rf "${XDG_DATA_HOME:-$HOME/.local}/periodic-table"
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database "$DESKTOPDIR" >/dev/null 2>&1 || true
fi
echo "==> 卸载完成（用户缓存中的字体配置保留，可手动删除 ~/.cache/periodic-table）"
