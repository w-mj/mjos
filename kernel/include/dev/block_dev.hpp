//
// Created by wmj on 3/19/20.
//

#ifndef OS_BLOCK_DEV_HPP
#define OS_BLOCK_DEV_HPP

#include <types.h>

namespace Dev {
    class BlockDevice {
    public:
        virtual _u32 read(char *buf, _u32 pos, _u32 size) = 0;
        virtual _u32 write(const char *buf, _u32 pos, _u32 size) = 0;
        // virtual _u32 tell()=0;
        // virtual void seek(_u32 pos)=0;
        // virtual void open(const std::string& path) = 0;
        // virtual void close() = 0;
    };
}


#endif //OS_BLOCK_DEV_HPP
