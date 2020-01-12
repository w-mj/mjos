![](./figure/x86_64_paging.svg)

每个页表项的格式如下表：

| Bit(s) | Name                  | Meaning                                                      |
| ------ | --------------------- | ------------------------------------------------------------ |
| 0      | present               | the page is currently in memory                              |
| 1      | writable              | it's allowed to write to this page                           |
| 2      | user accessible       | if not set, only kernel mode code can access this page       |
| 3      | write through caching | writes go directly to memory                                 |
| 4      | disable cache         | no cache is used for this page                               |
| 5      | accessed              | the CPU sets this bit when this page is used                 |
| 6      | dirty                 | the CPU sets this bit when a write to this page occurs       |
| 7      | huge page/null        | must be 0 in P1 and P4, creates a 1GiB page in P3, creates a 2MiB page in P2 |
| 8      | global                | page isn't flushed from caches on address space switch (PGE bit of CR4 register must be set) |
| 9-11   | available             | can be used freely by the OS                                 |
| 12-51  | physical address      | the page aligned 52bit physical address of the frame or the next page table |
| 52-62  | available             | can be used freely by the OS                                 |
| 63     | no execute            | forbid executing code on this page (the NXE bit in the EFER register must be set) |