# 一个玩具操作系统

## 搭建开发环境

1. 下载[gcc-9.3.0](https://mirrors.tuna.tsinghua.edu.cn/gnu/gcc/gcc-9.3.0/gcc-9.3.0.tar.xz), [bintuils-2.34](https://mirrors.tuna.tsinghua.edu.cn/gnu/binutils/binutils-2.34.tar.gz), [newlib-3.3.0](https://github.com/bminor/newlib/archive/newlib-3.3.0.tar.gz)，并解压到~/build-toolchain
2. 将目录下的三个patch文件应用到以上三个源代码中
3. 执行./build.sh all
4. 如果修改了newlib/libc/sys/mjos/syscall.c，需要重写编译newlib，执行./build.sh newlib
5. 如果在newlib/libc/sys/mjos/中新增或删除了源文件，需要在该目录下使用[autoconf-2.64](https://mirrors.tuna.tsinghua.edu.cn/gnu/autoconf/autoconf-2.64.tar.xz)和[automake-1.11](https://mirrors.tuna.tsinghua.edu.cn/gnu/automake/automake-1.11.tar.gz)执行autoreconf

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
