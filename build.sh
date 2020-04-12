#!/bin/bash
export SRC="$HOME/build-toolchain"

set -e
export PROJECT_PATH="$(pwd)"
export PREFIX=`realpath "$PROJECT_PATH/opt"`
export TARGET=x86_64-mjos
export PATH="$PREFIX/bin:$PATH"
export SYSROOT=`realpath $PROJECT_PATH/sysroot`
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PREFIX/lib
export ELF_TARGET=x86_64-elf

mjos_cxx() {
	cd build-gcc
	../gcc-9.3.0/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot=$SYSROOT --enable-languages=c,c++ --enable-shared
	make all-target-libstdc++-v3
	make install-target-libstdc++-v3
	cd ..
}

mjos_gcc() {
	mkdir -p build-gcc
	cd build-gcc
	../gcc-9.3.0/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot=$SYSROOT --enable-languages=c,c++ --enable-shared
	make all-gcc
	make all-target-libgcc
	make install-gcc
	make install-target-libgcc
	cd ..
}

mjos_binutils() {
	mkdir -p build-binutils
	cd build-binutils
	../binutils-2.34/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot=$SYSROOT --enable-shared
	make
	make install
	cd ..
}

newlib() {
	 # rm -r build-newlib
	 mkdir -p build-newlib
	 cd build-newlib
	 ../newlib-3.3.0/configure --prefix=/usr --target=$TARGET CFLAGS="-m64"
	 make all
	 make DESTDIR=${SYSROOT} install
	 cp -ar $SYSROOT/usr/x86_64-mjos/* $SYSROOT/usr/
	 rm -rf $SYSROOT/usr/x86_64-mjos/
	 cd ..
}

elf_binutils() {
	mkdir -p build-elf-binutils
	cd build-elf-binutils
	../binutils-2.34/configure --target=$ELF_TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
	make
	make install
	cd ..
}

elf_gcc() {
	mkdir -p build-elf-gcc
	cd build-elf-gcc
	../gcc-9.3.0/configure --target=$ELF_TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
	make all-gcc
	make all-target-libgcc | tee x86_64-elf/libgcc/lib.txt
	make install-gcc
	make install-target-libgcc
	cd x86_64-elf/libgcc
	grep crtstuff.c lib.txt | awk '{print($0" -mcmodel=large"); system($0" -mcmodel=large")}'
	cp crtbegin.o crtend.o $PROJECT_PATH/kernel/
	cd ../../..
}

cd $SRC
case $1 in
  "elf")
	  elf_binutils
	  elf_gcc
	;;
  "elf_gcc")
    elf_gcc
    ;;
  "elf_binutils")
    elf_binutils
    ;;
  "mjos")
	  mjos_gcc
	  mjos_newlib
	  mjos_cxx
	  ;;
  "gcc")
	  mjos_gcc
	;;
  "binutils")
	  mjos_binutils
	  ;;
  "newlib")
	  newlib
	  ;;
  "cxx")
	  mjos_cxx
	  ;;
	"all")
	  elf_binutils
	  elf_gcc
	  mjos_gcc
	  make mksysroot
	  mjos_newlib
	  mjos_cxx
	  ;;
esac
cd $PROJECT_PATH
