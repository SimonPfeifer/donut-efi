# donut-efi
Inspired by [a1kon's donut](https://www.a1k0n.net/2006/09/15/obfuscated-c-donut.html), this builds a UEFI `.efi` executable that renders a spinning donut in the UEFI shell.

## Depndencies
This project uses [gnu-efi](https://github.com/ncroxon/gnu-efi) to build the executable. This is then ran on a virtual machine using [qemu](https://www.qemu.org/).

You will also need `make`, `gcc`, `objcopy` and `ld`. 

## Build and run
After cloning this repository, clone `gnu-efi` into the main directory.
```sh
cd donut-efi
git clone https://github.com/ncroxon/gnu-efi.git
```

Follow `gnu-efi`'s instructions to build. Should just be:
```sh
cd gnu-efi
make
cd ..
```

Finally, to build and run the UEFI executable:
```sh
make run
```

## References
- [donut maths](https://www.a1k0n.net/2011/07/20/donut-math.html)
- [cosine from scratch](https://austinhenley.com/blog/cosine.html)
