#!/usr/bin/env bash
# 构建 Debian/Ubuntu .deb 安装包（amd64）
# 用法: bash build-deb.sh <版本号> [源码目录]
# 例:   bash build-deb.sh 1.0.0
set -euo pipefail

VERSION="${1:?用法: build-deb.sh <版本号> [源码目录]}"
SRC="${2:-/mnt/c/Projects/C++/元素周期表}"
WORK="/tmp/pt-deb-build"
PKGNAME="periodictable-pro"
PKGVER="$VERSION"
ARCH="amd64"
FULLNAME="${PKGNAME}_${PKGVER}_${ARCH}"
ROOT="$WORK/$FULLNAME"

echo "==> 构建 ${FULLNAME}.deb"

rm -rf "$WORK"
mkdir -p "$ROOT/DEBIAN" \
         "$ROOT/usr/bin" \
         "$ROOT/usr/lib/periodic-table" \
         "$ROOT/usr/share/applications" \
         "$ROOT/usr/share/icons/hicolor/256x256/apps" \
         "$ROOT/usr/share/doc/periodictable-pro"

# ---- 从自包含 tarball 取内容（保证与 Release 二进制一致）----
# 优先使用兼容老系统(glibc>=2.28)的新便携包；没有则回退旧包
NEWTAR="$SRC/dist/PeriodicTable-${VERSION}-linux-x64.tar.gz"
OLDTAR="$SRC/dist/PeriodicTable-v${VERSION}-linux-x64.tar.gz"
if [ -f "$NEWTAR" ]; then
  TARBALL="$NEWTAR"
else
  TARBALL="$OLDTAR"
fi
if [ ! -f "$TARBALL" ]; then
  echo "找不到 $TARBALL" >&2
  exit 1
fi
UNPACK="$WORK/unpack"
mkdir -p "$UNPACK"
tar -xzf "$TARBALL" -C "$UNPACK"
PAYLOAD=$(find "$UNPACK" -maxdepth 1 -type d -name 'PeriodicTable-*-linux-x64' | head -1)

cp "$PAYLOAD/PeriodicTable"   "$ROOT/usr/lib/periodic-table/"
cp -a "$PAYLOAD/lib"          "$ROOT/usr/lib/periodic-table/"
cp -a "$PAYLOAD/fonts"        "$ROOT/usr/lib/periodic-table/"
# 新布局：顶层 plugins/；旧布局：lib/plugins/。两者都兼容复制
[ -d "$PAYLOAD/plugins" ] && cp -a "$PAYLOAD/plugins" "$ROOT/usr/lib/periodic-table/"
cp "$PAYLOAD/run.sh"          "$ROOT/usr/lib/periodic-table/"
chmod +x "$ROOT/usr/lib/periodic-table/run.sh"
cp "$PAYLOAD/README.md"       "$ROOT/usr/share/doc/periodictable-pro/README.md"
cp "$PAYLOAD/periodic-table.desktop" "$ROOT/usr/share/applications/periodic-table.desktop"
cp "$PAYLOAD/periodic-table.png"     "$ROOT/usr/share/icons/hicolor/256x256/apps/periodic-table.png"

chmod 755 "$ROOT/usr/lib/periodic-table/PeriodicTable"

# ---- 启动脚本 ----
cat > "$ROOT/usr/bin/PeriodicTable" <<'EOF'
#!/bin/sh
# 元素周期表 Pro 启动器（自包含运行环境）
exec /usr/lib/periodic-table/run.sh "$@"
EOF
chmod 755 "$ROOT/usr/bin/PeriodicTable"

# ---- control 元数据 ----
INSTALLED_KB=$(du -sk "$ROOT/usr" | cut -f1)
cat > "$ROOT/DEBIAN/control" <<EOF
Package: $PKGNAME
Version: $PKGVER
Section: science
Priority: optional
Architecture: $ARCH
Depends: libc6 (>= 2.28), libstdc++6, libgcc-s1
Installed-Size: $INSTALLED_KB
Maintainer: Logic <3846190707@qq.com>
Homepage: https://github.com/WallfacingLogic/PeriodicTable-Pro
Description: Professional interactive periodic table (Qt6/C++)
 A complete, modern interactive periodic table with all 118 elements.
 .
 Features: full element data (Chinese/English names, atomic mass, electron
 configuration, electronegativity, density, melting/boiling points, etc.),
 multi-select category filters, instant search, glassmorphism UI, custom
 title bar, smooth scaling, HiDPI support.
 .
 This package is self-contained: Qt6 libraries, X11 plugins and a CJK font
 subset are bundled, so no system Qt or Chinese font installation is needed.
EOF

# ---- 安装后脚本 ----
cat > "$ROOT/DEBIAN/postinst" <<'EOF'
#!/bin/sh
set -e
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database /usr/share/applications >/dev/null 2>&1 || true
fi
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -f -t /usr/share/icons/hicolor >/dev/null 2>&1 || true
fi
exit 0
EOF
chmod 755 "$ROOT/DEBIAN/postinst"

# ---- 卸载后脚本 ----
cat > "$ROOT/DEBIAN/postrm" <<'EOF'
#!/bin/sh
set -e
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database /usr/share/applications >/dev/null 2>&1 || true
fi
exit 0
EOF
chmod 755 "$ROOT/DEBIAN/postrm"

# ---- copyright ----
cat > "$ROOT/usr/share/doc/periodictable-pro/copyright" <<'EOF'
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: PeriodicTable Pro
Source: https://github.com/WallfacingLogic/PeriodicTable-Pro

Files: *
Copyright: 2026 WallfacerLogic
License: MIT
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 .
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 .
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 .
 Files: usr/lib/periodic-table/fonts/*
 Copyright: Google Inc. and contributors
 License: OFL-1.1
 Noto Sans CJK SC subset, licensed under SIL Open Font License 1.1.
EOF

# ---- 打包 ----
mkdir -p "$SRC/dist"
OUT="$SRC/dist/${FULLNAME}.deb"
dpkg-deb --build --root-owner-group "$ROOT" "$OUT"

echo "==> 完成: $OUT"
dpkg-deb --info "$OUT" | head -20
ls -lh "$OUT"
