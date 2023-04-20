#ifndef IOFPRB_PARTINFO_H
#define IOFPRB_PARTINFO_H
#include <string>
#include "../utils/typedefs.h"

// partition info
struct partinfo
{
    explicit partinfo(std::string nm, bool is_log, bool is_prim) noexcept
            : name(std::move(nm)), is_primary(is_prim), is_logical(is_log),
              size(0), flags(0), type(0)
    {}

    std::string name;
    bool is_primary;
    bool is_logical;
    unsigned int sector_cnt;
    ull size;
    std::string partition_type_guid;
    ull flags;
    std::string uuid;
    int type;
    std::size_t offset;
    bool is_swap;

    std::string partition_type;
    std::size_t free;
    std::string fstype;
    std::string mntroot;
};

#endif //IOFPRB_PARTINFO_H
