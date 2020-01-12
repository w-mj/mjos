#### MSR寄存器

MSR寄存器是用于控制CPU状态的寄存器

可以通过rdmsr讲msr寄存器中的内容复制到eax中，通过wrmsr将eax中的内容复制到msr中。

| Bit(s) | Label | Description                    |
| ------ | ----- | ------------------------------ |
| 0      | SCE   | System Call Extensions         |
| 1-7    | 0     | Reserved                       |
| 8      | LME   | Long Mode Enable               |
| 10     | LMA   | Long Mode Active               |
| 11     | NXE   | No-Execute Enable              |
| 12     | SVME  | Secure Virtual Machine Enable  |
| 13     | LMSLE | Long Mode Segment Limit Enable |
| 14     | FFXSR | Fast FXSAVE/FXRSTOR            |
| 15     | TCE   | Translation Cache Extension    |
| 16-63  | 0     | Reserved                       |