#ifndef IOFPRB_FAT_H
#define IOFPRB_FAT_H

#include <unistd.h>
#include <fcntl.h>
#include "../utils/debug.h"
#include "../disk/partinfo.h"
#include <memory>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#define VFATFS "vfat"
#define FAT32FS "fat32"
#define exFAT "exfat"

#define FAT32_ID_STRING "FAT32   "

namespace fs {

bool probe_fat_fs(int fd, partinfo* partition)
{
    uint8_t buf[512];
    ASSERT(read(fd, buf, sizeof(buf)) > 0);
    if(buf[511] == 0xAA && buf[510] == 0x55)
        partition->is_boot = true;

    // at first check the "Bytes per sector" within BPB
    if(buf[0x0B] == 0 && buf[0x0C] == 0)
        partition->fstype = exFAT;
    else if (buf[0x36] == 0x29 && buf[0x37] == 0x05)
        partition->fstype = VFATFS;
    else if(std::memcmp(buf+0x52, FAT32_ID_STRING, 8) == 0)
        partition->fstype = FAT32FS;

    return !partition->fstype.empty();
}

}


#endif //IOFPRB_FAT_H
