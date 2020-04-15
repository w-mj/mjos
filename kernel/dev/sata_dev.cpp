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
    if (dirty) {
        logd("write to disk sector:%d", k);
        sata_write(sata, l32(k), h32(k), 1, virt_to_pfn(cache));
    }
});

_u32 Dev::SataDev::read(char *buf, _u32 pos, _u32 size) {
    // 从SATA磁盘读入数据，目前是按块分批次读取，因此不存在物理地址不连续的问题。
    // 改成一次读取多个块后需要考虑得到连续的物理页面
    u32 read_cnt = 0;
    while (size > 0) {
        u64 round_pos = ROUND_DOWN(pos, SECTOR_SIZE);  // 按扇区对齐
        u32 wasted = pos - round_pos;                  // 为了对齐多读的部分
        u64 sector = round_pos / SECTOR_SIZE;          // 待读扇区号
        auto t = pool.get(sector);
        char *temp = t.first;                 // 缓存
//        logd("sata dev sector: %d, cnt %d", sector, t.second);
//        if (sector == 1068) {
//            temp = t.first;
//        }
        // _si(t.second);
        if (t.second == 0) {
            // 只有这个缓存是第一次访问才读取磁盘
            sata_read(sata, l32(sector), h32(sector), 1, virt_to_phys(temp));
        }
        size_t read_this = os::min(size, SECTOR_SIZE - wasted);
        memcpy((void *)(buf + read_cnt), (void *)(temp + wasted), read_this);
        read_cnt += read_this;
        pos += read_this;
        size = size - read_this;
    }
    return read_cnt;
}


_u32 Dev::SataDev::write(const char *buf, _u32 pos, _u32 size) {
    u32 write_cnt = 0;
    while (size > 0) {
        u64 round_pos = ROUND_DOWN(pos, SECTOR_SIZE);  // 按扇区对齐
        u32 wasted = pos - round_pos;                  // 为了对齐多读的部分
        u64 sector = round_pos / SECTOR_SIZE;          // 待读扇区号
        auto t = pool.get(sector, true);
        char *temp = t.first;                 // 缓存
        size_t write_this = os::min(size, SECTOR_SIZE - wasted);
        if (t.second == 0 && write_this < SECTOR_SIZE) {
            // 第一次取到这个缓存，而且这次不能完整写入一个扇区，需要将这个扇区中的内容先读进来
            sata_read(sata, l32(sector), h32(sector), 1, virt_to_phys(temp));
        }
        memcpy((void *)(buf + write_cnt), (void *)(temp + wasted), write_this); // 把数据写入缓存
        write_cnt += write_this;
        pos += write_this;
        size = size - write_this;
    }
    return write_cnt;
}
