#!/usr/bin/env bash
# run.sh — build (if needed) and launch Nyanix in QEMU
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"

# Rebuild if os.img is missing or any source is newer
REBUILD=0
if [ ! -f "$ROOT/os.img" ]; then
    REBUILD=1
elif find "$ROOT/boot" -name "*.c" -o -name "*.asm" -o -name "*.h" 2>/dev/null \
     | xargs ls -t 2>/dev/null | head -1 \
     | xargs -I{} test "{}" -nt "$ROOT/os.img" 2>/dev/null; then
    REBUILD=1
fi

if [ "$REBUILD" -eq 1 ]; then
    echo "[run] Sources changed, rebuilding..."
    "$ROOT/scripts/build.sh"
fi

echo "[run] Launching QEMU..."
qemu-system-x86_64 \
    -drive format=raw,file="$ROOT/os.img" \
    -m 32M \
    -vga std \
    -no-reboot \
    "$@"
