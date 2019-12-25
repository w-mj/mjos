
IMG_NAME=boot.img
makeimg:
ifeq ("$(wildcard $($(IMG_NAME)))","")
	dd if=/dev/zero of=$(IMG_NAME) bs=1M count=128
	mkfs.vfat $(IMG_NAME)
	mmd -i $(IMG_NAME) boot
	mmd -i $(IMG_NAME) boot/efi
endif

copyimg:
	mcopy -i $(IMG_NAME) boot.efi ::/boot/efi