//
// Created by wmj on 3/20/20.
//

#include <dev/sata_dev.hpp>
#include <dev/ahci.h>
#include <lru.hpp>
#include <boot.h>
#include <algorithm.hpp>
#include <memory/kmalloc.h>

constexpr int SECTOR_SIZE = 512;
os::LRU<int, char *> pool(10,
        [](int k){return (char *)kmalloc_s(SECTOR_SIZE);},
        [](int k, char *cache) {kfree_s(SECTOR_SIZE, cache);},
        [](int k, char *cache, bool dirty) {
    // TODO: 在这里写入磁盘
});

_u32 Dev::SataDev::read(MM::Buf &buf, _u32 pos, _u32 size) {

    u32 read_cnt = 0;
    while (size > 0) {
        u64 round_pos = ROUND_DOWN(pos, SECTOR_SIZE);  // 按扇区对齐
        u32 wasted = pos - round_pos;                  // 为了对齐多读的部分
        u64 sector = round_pos / SECTOR_SIZE;          // 待读扇区号
        auto t = pool.get(sector);
        char *temp = t.first;                 // 缓存
        _si(t.second);
        if (t.second == 0) {
            // 只有这个缓存是第一次访问才读取磁盘
            sata_read(sata, l32(sector), h32(sector), 1, virt_to_phys(temp));
        }
        u32 read_this = os::min(size, SECTOR_SIZE - wasted);
        memcpy(buf.data + read_cnt, temp + wasted, read_this);
        read_cnt += read_this;
        pos += read_this;
        size = size - read_this;
    }
    return read_cnt;
}


_u32 Dev::SataDev::write(MM::Buf &buf, _u32 pos, _u32 size) {
    return 0;
}
