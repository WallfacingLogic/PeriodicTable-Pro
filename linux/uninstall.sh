#!/usr/bin/env bash
# 元素周期表 Pro — 卸载脚本（自包含版）
set -e
PREFIX="${PREFIX:-/usr/local}"
echo "==> 卸载元素周期表 Pro ..."
rm -f "$PREFIX/bin/PeriodicTable" \
      "$PREFIX/share/applications/periodic-table.desktop" \
      "$PREFIX/share/icons/hicolor/256x256/apps/periodic-table.png"
rm -rf "$PREFIX/lib/periodic-table"
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database "$PREFIX/share/applications" 2>/dev/null || true
fi
echo "==> 卸载完成。"
