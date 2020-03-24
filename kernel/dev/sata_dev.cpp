//
// Created by wmj on 3/20/20.
//

#include <dev/sata_dev.hpp>
#include <dev/ahci.h>
// #include <lru.hpp>
#include <stdlib.h>
#include <boot.h>
#include <algorithm.hpp>

constexpr int SECTOR_SIZE = 512;
_u32 Dev::SataDev::read(MM::Buf &buf, _u32 pos, _u32 size) {
    // static std::LRU<int, char *> pool(10, [](int k) {return new char[SECTOR_SIZE];});
    static char *temp = NULL;
    if (temp == NULL) {
        temp = static_cast<char *>(malloc(SECTOR_SIZE));
    }
    u32 read_cnt = 0;
    while (size > 0) {
        u64 round_pos = ROUND_DOWN(pos, SECTOR_SIZE);  // 按扇区对齐
        u32 wasted = pos - round_pos;                  // 为了对齐多读的部分
        u64 sector = round_pos / SECTOR_SIZE;          // 待读扇区号
        // char *temp = pool.get(sector);                 // 缓存
        sata_read(sata, l32(sector), h32(sector), 1, virt_to_phys(temp));
        u32 read_this = std::min(size, SECTOR_SIZE - wasted);
        memcpy(buf.data + read_cnt, temp + wasted, read_this);
        read_cnt += read_this;
        size = size - read_this;
    }
    return read_cnt;
}


_u32 Dev::SataDev::write(MM::Buf &buf, _u32 pos, _u32 size) {
    return 0;
}
