//
// Created by wmj on 3/20/20.
//

#ifndef OS_SATA_DEV_H
#define OS_SATA_DEV_H

#include <dev/block_dev.hpp>

namespace Dev {
    class SataDev : public Dev::BlockDevice {
        _u32 write(MM::Buf &buf, _u32 pos, _u32 size) override;
        _u32 read(MM::Buf &buf, _u32 pos, _u32 size) override;
    };
}

#endif //OS_SATA_DEV_H
