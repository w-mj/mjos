#!/bin/bash

gdb -q --readnow -ex "symbol-file kernel/kernel.sym" -ex "target remote :4444"
