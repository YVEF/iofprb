#ifndef IOFPRB_DISKCTX_H
#define IOFPRB_DISKCTX_H
#include <string>
#include "../utils/typedefs.h"
#include <vector>
#include "partinfo.h"

// disk info context
struct diskctx
{
    explicit diskctx(std::string dev) noexcept : devname(std::move(dev)) {}

    const std::string devname;
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
