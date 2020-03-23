//
// Created by wmj on 3/20/20.
//

#include <dev/sata_dev.hpp>
#include <dev/ahci.h>
// #include <lru.hpp>
#include <stdlib.h>

constexpr int SECTOR_SIZE = 512;
_u32 Dev::SataDev::write(MM::Buf &buf, _u32 pos, _u32 size) {
    // static std::LRU<int, char *> pool(10, [](int k) {return new char[SECTOR_SIZE];});
    static char *temp = NULL;
    if (temp == NULL) {
        temp = static_cast<char *>(malloc(SECTOR_SIZE));
    }
    while (size) {
        u64 round_pos = ROUND_DOWN(pos, SECTOR_SIZE);  // 按扇区对齐
        u64 wasted = pos - round_pos;                  // 为了对齐多读的部分
        u64 sector = round_pos / SECTOR_SIZE;          // 待读扇区号
        // char *temp = pool.get(sector);                 // 缓存
        sata_read(sata, l32(sector), h32(sector), 1, (u64) temp);
        size = size - SECTOR_SIZE - (pos - round_pos);
    }
}


_u32 Dev::SataDev::read(MM::Buf &buf, _u32 pos, _u32 size) {
    return 0;
}
