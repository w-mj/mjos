#!/bin/bash

qemu-system-x86_64 -cpu qemu64 -bios OVMF.fd -drive file=buildtools/boot.img,if=ide -net none

