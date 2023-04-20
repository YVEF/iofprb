#ifndef IOFPRB_DRIVEPRV_H
#define IOFPRB_DRIVEPRV_H

#include <unordered_map>
#include "../disk/diskctx.h"

namespace providers {

struct disk_draw_info
{
    std::string name;
    std::string description;
    const std::vector<partinfo*>* partitions;
};

class driveprv
{
    std::unordered_map<std::string, diskctx*> disks_;

public:
    explicit driveprv(const std::vector<diskctx*>&) noexcept;
    [[nodiscard]] std::vector<disk_draw_info> get_drives_info() const noexcept;

};

} // providers

#endif //IOFPRB_DRIVEPRV_H
