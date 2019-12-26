#!/bin/bash

dd if=/dev/zero of=boot.img bs=1M count=128
parted boot.img -s -a minimal mklabel gpt
parted boot.img -s -a minimal mkpart EFI FAT32 2048s 93716s
parted boot.img -s -a minimal toggle 1 boot
# mformat -i boot.img -f 1440 ::
# mmd -i boot.img ::/EFI
# mmd -i boot.img ::/EFI/BOOT
# mcopy -i boot.img $(EFI_NAME) ::/EFI/BOOT 
# mkgpt -o$(DISK_NAME) --image-size 4096 --part boot.img --type system
dd if=/dev/zero of=/tmp/part.img bs=512 count=91669
mformat -i /tmp/part.img -h 32 -t 32 -n 64 -c 1
mcopy -i /tmp/part.img ../build/boot/efi/helloefi.efi ::
dd if=/tmp/part.img of=boot.img bs=512 count=91669 seek=2048 conv=notrunc


