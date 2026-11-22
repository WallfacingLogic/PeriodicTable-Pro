#!/usr/bin/env bash
# 元素周期表 Pro — 本地运行入口（免安装，自包含）
# 若当前目录未解压依赖，则从自包含 tar.gz 解压到 .runtime 后运行。
DIR="$(cd "$(dirname "$0")" && pwd)"
if [ -d "$DIR/lib" ] && [ -d "$DIR/fonts" ] && [ -f "$DIR/PeriodicTable" ]; then
  exec "$DIR/run-selfcontained.sh" "$@"
fi
RUNTIME="$DIR/.runtime"
if [ ! -f "$RUNTIME/periodic-table-linux-selfcontained/run.sh" ]; then
  mkdir -p "$RUNTIME"
  tar -xzf "$DIR/periodic-table-linux-selfcontained.tar.gz" -C "$RUNTIME"
fi
chmod +x "$RUNTIME/periodic-table-linux-selfcontained/run.sh" "$RUNTIME/periodic-table-linux-selfcontained/PeriodicTable" 2>/dev/null || true
exec "$RUNTIME/periodic-table-linux-selfcontained/run.sh" "$@"
