#!/usr/bin/env bash
# 元素周期表 Pro — 自包含运行脚本（无需系统安装 Qt / 中文字体）
# 用法: ./run.sh  或  ./run.sh --screenshot out.png
DIR="$(cd "$(dirname "$0")" && pwd)"

# Qt6 库与插件
export LD_LIBRARY_PATH="$DIR/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="$DIR/lib/plugins${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}"
export QT_QPA_PLATFORM_PLUGIN_PATH="$DIR/lib/plugins/platforms"

# 私有 fontconfig：内置 Noto Sans CJK SC 覆盖全部 118 元素中文名
# 配置与缓存写入用户私有目录，避免系统安装后普通用户无写权限
CACHE_BASE="${XDG_CACHE_HOME:-$HOME/.cache}/periodic-table"
mkdir -p "$CACHE_BASE" 2>/dev/null || CACHE_BASE="/tmp/periodic-table-cache-$(id -u)"
mkdir -p "$CACHE_BASE" 2>/dev/null || true
CONF="$CACHE_BASE/fonts.conf"
cat > "$CONF" <<EOF
<?xml version="1.0"?>
<!DOCTYPE fontconfig SYSTEM "fonts.dtd">
<fontconfig>
  <dir>$DIR/fonts</dir>
  <cachedir>$CACHE_BASE/fontcache</cachedir>
  <match target="pattern">
    <test name="lang" compare="contains"><string>zh</string></test>
    <edit name="family" mode="prepend" binding="strong"><string>Noto Sans CJK SC</string></edit>
  </match>
  <match target="pattern">
    <test qual="any" name="family"><string>sans-serif</string></test>
    <edit name="family" mode="prepend" binding="strong"><string>Noto Sans CJK SC</string></edit>
  </match>
  <match target="pattern">
    <test qual="any" name="family"><string>WenQuanYi Micro Hei</string></test>
    <edit name="family" mode="prepend" binding="strong"><string>Noto Sans CJK SC</string></edit>
  </match>
</fontconfig>
EOF
export FONTCONFIG_FILE="$CONF"

exec "$DIR/PeriodicTable" "$@"
