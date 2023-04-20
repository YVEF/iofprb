#include "driveprv.h"
#include <algorithm>
#include <sstream>

namespace providers {

static disk_draw_info build_disk_info(const std::string& name, const diskctx* disk)
{
    std::stringstream str;
    str << "Disk Id: " << disk->disk_id << std::endl
            << "Model: " << disk->model << std::endl
        << "Serial No: " << disk->model << std::endl
        << "Sector size (min/opt): " << disk->min_sector_size << "b / "
        << disk->optimal_sector_size << "b" << std::endl
        << "System partition: " << disk->name << std::endl
        << "Partition table: " << disk->partition_table << std::endl;

    disk_draw_info info{};
    info.name = name;
    info.description = str.str();
    info.partitions = &disk->partitions;
    return info;
}

std::vector<disk_draw_info> driveprv::get_drives_info() const noexcept
{
    std::vector<disk_draw_info> result;
    std::transform(disks_.begin(), disks_.end(), std::back_inserter(result),
                   [](const std::pair<std::string, diskctx*>& p)
                   { return build_disk_info(p.first, p.second); });
   return result;
}

driveprv::driveprv(const std::vector<diskctx*>& dsk) noexcept
{
    for(auto d : dsk)
        disks_.emplace(d->model, d);
}

} // providers