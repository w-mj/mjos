
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

all: build

GCC_FLAGS=-fno-stack-protector -Wall -fpic -fshort-wchar -mno-red-zone 
LD_FLAGS=-nostdlib -znocombreloc

INC_PATH=$(EFI_HEADERS) $(EFI_HEADERS_X86_64)
INC_FLAG=$(addprefix -I, $(INC_PATH))

OBJCPY_FLAG=text sdata data dynamic rel rela reloc
OBJCPY_FLAGS=$(patsubst %, -j .%, $(OBJCPY_FLAG))

build: .depend $(EFI_NAME)

.depend: $(C_NAME)
	gcc -MM $^ > $@
	sed -i '/.o:/ s,^,,' $@

sinclude .depend

$(EFI_NAME): $(O_NAME)
	ld $< $(LIB_DIR)/crt0-efi-x86_64.o  -nostdlib  -znocombreloc -T $(LIB_DIR)/elf_x86_64_efi.lds -shared -Bsymbolic -L $(LIB_DIR) -l:libgnuefi.a -l:libefi.a -o $(SO_NAME)
	objcopy $(OBJCPY_FLAGS) --target=efi-app-x86_64 $(SO_NAME) $@


%.o: %.c $(MAKEFILE_LIST)
	gcc -c $< $(GCC_FLAGS) $(INC_FLAG) -DEFI_FUNCTION_WRAPPER -o $@

install:
	mkdir -p ../build/boot && cp $(EFI_NAME) ../build/boot/$(EFI_NAME)


clean:
	-rm -f *.so *.o *.efi .depend

.PHONY: build clean
