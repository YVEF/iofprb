#ifndef IOFPRB_DISKCTX_H
#define IOFPRB_DISKCTX_H
#include <string>
#include "../utils/typedefs.h"
#include <vector>
#include "partinfo.h"
#include <cassert>

namespace diskctx_internal {
// /dev/{disk} format
static
std::string to_short(const std::string& devname)
{
    auto idx = devname.find_last_of('/');
    assert(idx != std::string::npos);
    return devname.substr(idx + 1);
}
}

// disk info context
struct diskctx
{
    explicit diskctx(std::string dev) noexcept
    : devname(std::move(dev)),
    short_devname(diskctx_internal::to_short(devname)) {}

    const std::string devname;
    const std::string short_devname;
    ull size;
    unsigned int physical_sector_size;
    unsigned int logical_sector_size;
    unsigned int min_sector_size;
    unsigned int optimal_sector_size;
    std::size_t offset;
    unsigned int logical_sector_cnt;
    unsigned int physical_sector_cnt;
    unsigned long fs_blk_size;
    unsigned long fs_occup_blk_cnt;


    std::string disk_id;
    std::string partition_table;
    std::vector<partinfo*> partitions;

    std::string model;
    std::string serialno;

    ~diskctx() noexcept
    {
        for(auto pi : partitions)
            delete pi;
    }
};


#endif //IOFPRB_DISKCTX_H
