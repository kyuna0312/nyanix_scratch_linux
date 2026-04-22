#!/usr/bin/env bash
# setup.sh — install all build dependencies for Nyanix on Arch/Manjaro
set -euo pipefail

echo "[setup] Checking build dependencies..."

PKGS=()

check() {
    command -v "$1" &>/dev/null || PKGS+=("$2")
}

check nasm        nasm
check gcc         gcc
check ld          binutils
check objcopy     binutils
check qemu-system-x86_64  qemu-full

if [ ${#PKGS[@]} -eq 0 ]; then
    echo "[setup] All dependencies already installed."
    exit 0
fi

echo "[setup] Missing: ${PKGS[*]}"
echo "[setup] Installing via pacman..."
sudo pacman -Sy --needed --noconfirm "${PKGS[@]}"

echo "[setup] Done. Run ./scripts/build.sh to build."
