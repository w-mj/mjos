#!/usr/bin/python3

import sys

def main():
    n = sys.argv[1]
    n = int(n, 16)
    print("PML4 {}".format((n & (0x1ff << 39)) >> 39))
    print("PDP  {}".format((n & (0x1ff << 30)) >> 30))
    print("PDT  {}".format((n & (0x1ff << 21)) >> 21))
    print("PTE  {}".format((n & (0x1ff << 12)) >> 12))
    print("OFF  {}".format(n & (0xfff)))


if __name__ == '__main__':
    main()
