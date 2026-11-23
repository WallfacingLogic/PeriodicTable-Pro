#!/usr/bin/env bash
# 本地 Linux 发布包构建脚本
# 用法: bash scripts/build-linux-release.sh <版本号> [源码目录]
set -e

VERSION="${1:?用法: build-linux-release.sh <版本号>}"
SRC="${2:-/mnt/c/Projects/C++/元素周期表}"
WORK=/tmp/pt-release
NAME="PeriodicTable-${VERSION}-linux-x64"

echo "==> 构建 ${NAME}"
rm -rf "$WORK"
mkdir -p "$WORK/src/resources"
cp "$SRC"/*.cpp "$SRC"/*.h "$SRC"/CMakeLists.txt "$SRC"/resources.qrc "$WORK/src/"
cp "$SRC"/resources/* "$WORK/src/resources/"

echo "==> 编译"
cd "$WORK"
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release > /dev/null
cmake --build build -j"$(nproc)" > /dev/null

echo "==> 组装自包含包"
PKG="$WORK/dist/$NAME"
mkdir -p "$PKG/lib/plugins/platforms" "$PKG/fonts"
cp build/PeriodicTable "$PKG/"
cp src/resources/PeriodicTable.png "$PKG/periodic-table.png"

# 递归收集依赖
collect() {
  local target="$1"
  while read -r lib; do
    [ -f "$lib" ] || continue
    local b
    b=$(basename "$lib")
    case "$b" in
      libc.so.6|libm.so.6|libpthread.so.0|libdl.so.2|librt.so.1|libstdc++.so.6|libgcc_s.so.1|ld-linux*) continue ;;
    esac
    if [ ! -e "$PKG/lib/$b" ]; then
      cp -L "$lib" "$PKG/lib/$b"
      collect "$PKG/lib/$b"
    fi
  done < <(ldd "$target" 2>/dev/null | awk '/=> \// {print $3}')
}
collect "$PKG/PeriodicTable"

PLUGDIR=$(find /usr/lib -type d -path '*qt6/plugins/platforms' | head -1)
for plug in libqxcb.so libqoffscreen.so libqminimal.so; do
  [ -f "$PLUGDIR/$plug" ] && cp "$PLUGDIR/$plug" "$PKG/lib/plugins/platforms/"
done
for plug in "$PKG"/lib/plugins/platforms/*.so; do
  [ -f "$plug" ] && collect "$plug"
done

echo "==> 生成中文字体子集"
python3 - "$PKG" <<'PY'
import glob, sys, subprocess
pkg = sys.argv[1]
chars = set(chr(i) for i in range(0x20, 0x7f))
for c in "，。、；：？！“”‘’（）《》〈〉【】·—…￥％＃＆＠～²³¹⁰⁻×℃":
    chars.add(c)
for p in glob.glob('/tmp/pt-release/src/*.cpp') + glob.glob('/tmp/pt-release/src/*.h'):
    chars.update(ch for ch in open(p, encoding='utf-8').read() if ord(ch) > 127)
open('/tmp/pt-release/charset.txt', 'w', encoding='utf-8').write(''.join(sorted(chars)))
for w in ('Regular', 'Bold'):
    subprocess.run([
        sys.executable, '-m', 'fontTools.subset',
        f'/usr/share/fonts/opentype/noto/NotoSansCJK-{w}.ttc',
        '--font-number=2', '--text-file=/tmp/pt-release/charset.txt',
        f'--output-file={pkg}/fonts/NotoSansSC-{w}.otf',
        "--layout-features=*", '--no-hinting', '--desubroutinize', "--name-IDs=*",
    ], check=True)
PY

echo "==> 复制脚本与文档"
cp "$SRC"/linux/run-selfcontained.sh "$PKG/run.sh"
cp "$SRC"/linux/run.sh "$PKG/run-local.sh"
cp "$SRC"/linux/install.sh "$SRC"/linux/uninstall.sh "$PKG/"
cp "$SRC"/linux/periodic-table.desktop "$PKG/"
cp "$SRC"/linux/README.md "$PKG/README.md"
chmod +x "$PKG/PeriodicTable" "$PKG"/*.sh

echo "==> 打包"
cd "$WORK/dist"
tar -czf "$NAME.tar.gz" "$NAME"
sha256sum "$NAME.tar.gz" > SHA256SUMS.txt

OUT="$SRC/linux"
cp -f "$NAME.tar.gz" "$OUT/"
cp -f SHA256SUMS.txt "$OUT/"
echo "==> 完成"
ls -lh "$NAME.tar.gz" SHA256SUMS.txt
cat SHA256SUMS.txt
