
# IMG_NAME=boot.vhd
# DISK_NAME=DISK.vhd
NAME=helloefi

C_NAME=$(addsuffix .c, $(NAME))
O_NAME=$(addsuffix .o, $(NAME))
SO_NAME=$(addsuffix .so, $(NAME))
EFI_NAME=$(addsuffix .efi, $(NAME))

EFI_HEADERS=/usr/include/efi/
EFI_HEADERS_X86_64=$(addsuffix x86_64/, $(EFI_HEADERS))
LIB_DIR=/usr/lib

# makeimg:
# ifeq ("$(wildcard $($(IMG_NAME)))","")
# 	dd if=/dev/zero of=$(IMG_NAME) bs=1M count=128
# 	parted $(IMG_NAME) -s -a minimal mklabel gpt
# 	parted $(IMG_NAME) -s -a minimal mkpart EFI FAT32 2048s 93716s
# 	parted $(IMG_NAME) -s -a minimal toggle 1 boot
# 	# mformat -i $(IMG_NAME) -f 1440 ::
# 	# mmd -i $(IMG_NAME) ::/EFI
# 	# mmd -i $(IMG_NAME) ::/EFI/BOOT
# endif

# copyimg: makeimg build
# 	# mcopy -i $(IMG_NAME) $(EFI_NAME) ::/EFI/BOOT 
# 	# mkgpt -o$(DISK_NAME) --image-size 4096 --part $(IMG_NAME) --type system
# 	dd if=/dev/zero of=/tmp/part.img bs=512 count=91669
# 	mformat -i /tmp/part.img -h 32 -t 32 -n 64 -c 1
# 	mcopy -i /tmp/part.img $(EFI_NAME) ::
# 	dd if=/tmp/part.img of=$(IMG_NAME) bs=512 count=91669 seek=2048 conv=notrunc

GCC_FLAGS=-c -fno-stack-protector -Wall -fpic -fshort-wchar -mno-red-zone 
LD_FLAGS=-nostdlib -znocombreloc

OBJCPY_FLAG=text sdata data dynamic rel rela reloc
OBJCPY_FLAGS=$(patsubst %, -j .%, $(OBJCPY_FLAG))

build:
	gcc $(C_NAME) $(GCC_FLAGS) -I$(EFI_HEADERS) -I$(EFI_HEADERS_X86_64) -DEFI_FUNCTION_WRAPPER -o $(O_NAME)
	ld $(O_NAME) $(LIB_DIR)/crt0-efi-x86_64.o  -nostdlib  -znocombreloc -T $(LIB_DIR)/elf_x86_64_efi.lds -shared -Bsymbolic -L $(LIB_DIR) -l:libgnuefi.a -l:libefi.a -o $(SO_NAME)
	objcopy $(OBJCPY_FLAGS) --target=efi-app-x86_64 $(SO_NAME) $(EFI_NAME)

install:
	mkdir -p ../build/boot/efi && cp $(EFI_NAME) ../build/boot/efi/$(EFI_NAME)


clean:
	-rm -f *.so *.o *.efi

.PHONY: build clean