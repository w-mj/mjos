#!/bin/bash
dd if=/dev/zero of=disk.vhd bs=1M count=1024
echo "g
n


+128M
n



t
1
1
w

"| fdisk disk.vhd
# start = 2048*512=1048576
# size = 262144*512=134217728
losetup --offset 1048576 --sizelimit 134217728 /dev/loop0 disk.vhd