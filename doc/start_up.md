# 系统启动流程

1. 整个系统从boot.S 中的_start开始执行。
2. _start：（32位保护模式）
   1. 关中断
   2. 关闭分页（cr0的第31位置0   cr0 &= 0x7fffffff)
   3. 设置初始页目录表（cr3）
   4. 打开PAE模式
   5. 打开长模式
   6. 开启分页
   7. 读入段表
   8. 跳转到 entry64
3. entry64：（64位保护模式）
   1. 初始化段寄存器（全部指向2号段data0）
   2. 初始化FS和GS的高32位
   3. 跳转到higher_half
4. higher_half：
   1. 初始化堆栈
   2. 清空flags
   3. 调用C语言函数kernel_main启动内核

