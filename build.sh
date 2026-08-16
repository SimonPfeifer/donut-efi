# bin/bash

GNU_EFI_DIR=/home/simonpfeifer/Documents/efi/gnu-efi
EFI_HEADER=${GNU_EFI_DIR}/inc

BUILD_DIR=build

mkdir -p ${BUILD_DIR}

# Build
gcc -I${EFI_HEADER} -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -c src/main.c -o ${BUILD_DIR}/main.o

# Link
ld -shared -Bsymbolic -L${GNU_EFI_DIR}/x86_64/lib -L${GNU_EFI_DIR}/x86_64/gnuefi -T${GNU_EFI_DIR}/gnuefi/elf_x86_64_efi.lds ${GNU_EFI_DIR}/x86_64/gnuefi/crt0-efi-x86_64.o ${BUILD_DIR}/main.o -o ${BUILD_DIR}/main.so -lgnuefi -lefi

# To make the executable run on boot
mkdir -p ${BUILD_DIR}/EFI/BOOT
objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --output-target efi-app-x86_64 --subsystem=10 ${BUILD_DIR}/main.so ${BUILD_DIR}/EFI/BOOT/bootx64.efi

# To run the executable from the shell with "main"
#objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --output-target efi-app-x86_64 --subsystem=10 ${BUILD_DIR}/main.so ${BUILD_DIR}/main.efi
