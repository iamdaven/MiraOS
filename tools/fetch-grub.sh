#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
GRUB_DIR="$ROOT/tools/grub"
DEB="grub-pc-bin_2.12-1ubuntu7_amd64.deb"
URL="http://archive.ubuntu.com/ubuntu/pool/main/g/grub2/$DEB"

if [ -f "$GRUB_DIR/i386-pc/cdboot.img" ]; then
    exit 0
fi

mkdir -p "$GRUB_DIR"
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

curl -fsSL -o "$TMP/$DEB" "$URL"
cd "$TMP"
ar x "$DEB"
mkdir -p data
tar --zstd -xf data.tar.zst -C data ./usr/lib/grub/i386-pc
cp -r data/usr/lib/grub/i386-pc "$GRUB_DIR/"
echo "grub installed to $GRUB_DIR"
