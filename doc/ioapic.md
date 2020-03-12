IOAPIC 常用引脚连接
+ Pin #1连接到键盘中断（IRQ1）
+ Pin #2连接到IRQ0
+ Pin #3-#11,#14,#15，分别连接到ISA IRQ[3:7,8#,9:11,14:15]
+ Pin #12连接到鼠标中断（IRQ12/M）
+ Pin #16-#19代表PCI IRQ[0:3]
+ Pin #20-#21代表Motherboard IRQ[0:1]
+ Pin #23代表SMI中断，若Mask掉，则SMI中断会从IOAPIC的#SMIOUT引脚引出，否则会由IOAPIC根据RTE #23转发
