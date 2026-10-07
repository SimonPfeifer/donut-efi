BUILD_DIR := ./build

GNU_EFI_DIR := ./gnu-efi
EFI_HEADER = ${GNU_EFI_DIR}/inc

OVMF_PATH := /usr/share/ovmf/OVMF.fd

efi: link
	mkdir -p ${BUILD_DIR}/EFI/BOOT
	objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --output-target efi-app-x86_64 --subsystem=10 ${BUILD_DIR}/main.so ${BUILD_DIR}/EFI/BOOT/bootx64.efi

link: build
	ld -shared -Bsymbolic -L${GNU_EFI_DIR}/x86_64/lib -L${GNU_EFI_DIR}/x86_64/gnuefi -T${GNU_EFI_DIR}/gnuefi/elf_x86_64_efi.lds ${GNU_EFI_DIR}/x86_64/gnuefi/crt0-efi-x86_64.o ${BUILD_DIR}/main.o -o ${BUILD_DIR}/main.so -lgnuefi -lefi

build:
	mkdir -p ${BUILD_DIR}
	gcc -I${EFI_HEADER} -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -c src/main.c -o ${BUILD_DIR}/main.o

clean:
	rm -rf ${BUILD_DIR}

run: efi
	qemu-system-x86_64 \
		-nodefaults \
		-enable-kvm \
		-display gtk \
		-vga std \
		-bios  ${OVMF_PATH} \
		-drive format=raw,file=fat:rw:build

.PHONY: run clean efi link build all
