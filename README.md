# 毕业设计：一个玩具操作系统

## 搭建内核开发环境
1. 安装依赖 gcc bulitin make bison flex gmp mpfr mpc texinfo tee

2. 下载[gcc](https://ftp.gnu.org/gnu/gcc/)和[builtin](https://ftp.gnu.org/gnu/binutils/)的源码，并解压到~/src。

3. 设置环境变量
    ```shell script
    export PREFIX="/usr/local"
    export TARGET=x86_64-elf
    export PATH="$PREFIX/bin:$PATH"
    ```
    
4. 编译builtin
    ```shell script
    cd $HOME/src
    mkdir build-binutils
    cd build-binutils
    ../binutils-x.y.z/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
    make
    sudo make install
    ```
    
5. 编译gcc和libgcc，并备份编译libgcc的命令
   ```shell script
   cd $HOME/src
   mkdir build-gcc
   cd build-gcc
   ../gcc-x.y.z/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
   make all-gcc
   make all-target-libgcc | tee lib.txt
   sudo make install-gcc
   sudo make install-target-libgcc
   ```
	
6. 编译64位版本的crtbegin.o和crtend.o

    ```shell script
    grep crtstuff.c lib.txt  # 并复制输出的两条指令
    cd x86_64-elf/libgcc/
	  # 分别粘贴将第一步中输出的指令，并在末尾加上-mcmodel=large，执行
    cp crtbegin.o crtend.o ~/os/kernel/   # 将这两个文件复制到项目中的kernel的目录
    ```
    
7. 如果你修改了PREFIX，需要将根Makefile中的TOOLCHAIN_BASE也修改为正确的位置。

## 搭建用户开发环境

1. 将项目目录下diff文件合并进binutlis和gcc的源码中。
2. 使用--target=x86_64-mjos和--with-sysroot="sysroot"再次编译。

## 构建

### 编译和运行

```shell script
make run
```

### 调试

可以使用gdb连接到tcp:4444进行调试。

```shell script
make debug
```

