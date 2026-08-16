# bin/bash

qemu-system-x86_64 \
    -nodefaults \
    -enable-kvm \
    -display gtk \
    -vga std \
    -bios /usr/share/ovmf/OVMF.fd \
    -drive format=raw,file=fat:rw:build
