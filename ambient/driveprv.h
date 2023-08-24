#ifndef IOFPRB_DRIVEPRV_H
#define IOFPRB_DRIVEPRV_H

#include <unordered_map>
#include "../disk/diskctx.h"

namespace ambient {

struct disk_draw_info
{
    std::string name;
    std::string uuid;
    std::string description;
    const std::vector<partinfo*>* partitions;
};

class driveprv
{
public:
    explicit driveprv(const std::vector<diskctx*>&) noexcept;
    [[nodiscard]] std::vector<disk_draw_info> get_drives_info() const noexcept;
    const diskctx* get_disk(const std::string& uuid) const noexcept;

private:
    // uuid -> disk
    std::unordered_map<std::string, diskctx*> disks_;

};

} // providers

#endif //IOFPRB_DRIVEPRV_H
