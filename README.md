# 一个玩具操作系统

## 搭建开发环境

1. 下载gcc-9.3.0, bintuils-2.34, newlib-3.3.0，并解压到~/build-toolchain
2. 将目录下的三个patch文件应用到源代码中
3. 执行./build.sh all
4. 如果修改了newlib/libc/sys/mjos/syscall.c，需要重写编译newlib，执行./build.sh newlib
5. 如果在newlib/libc/sys/mjos/中新增或删除了源文件，需要在该目录下使用autoconf-2.64和automake-1.11执行autoreconf

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
