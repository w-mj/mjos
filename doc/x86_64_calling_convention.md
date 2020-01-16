# x86_64调用约定

x86_64中函数调用不再使用系统栈，所有参数和返回值都放在寄存器中。

对于整数和地址参数，从左至右放在RDI, RSI, RDX, RCX, R8, 和 R9中

对于浮点参数，分别放在XMM0-7八个寄存器中。

![](./figure/x86_64_calling_convention.jpg)