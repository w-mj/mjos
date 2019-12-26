#!/bin/bash

make
make install
./mkboot.sh

qemu-system-x86_64 -cpu qemu64 -bios OVMF.fd -drive file=boot.img,if=ide -net none

