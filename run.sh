#!/bin/bash
# Run Nyanix in QEMU (auto-exits after 60s)
./build.sh
qemu-system-x86_64 -drive format=raw,file=os.img -m 32M -vga std -display gtk &
sleep 60
echo "Done (60s timeout)"