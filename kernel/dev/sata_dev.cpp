//
// Created by wmj on 3/20/20.
//

#include <dev/sata_dev.hpp>
#include <dev/ahci.h>
#include <lru.hpp>

_u32 Dev::SataDev::write(MM::Buf &buf, _u32 pos, _u32 size) {
    return 0;
}


_u32 Dev::SataDev::read(MM::Buf &buf, _u32 pos, _u32 size) {
    return 0;
}
