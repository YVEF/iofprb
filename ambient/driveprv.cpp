#include "driveprv.h"
#include <algorithm>
#include <sstream>

namespace ambient {

static disk_draw_info build_disk_info(const diskctx* disk)
{
    std::stringstream str;
    str << "Disk Id: " << disk->disk_id << "\n"
        << "Model: " << disk->model << "\n"
        << "Serial No: " << disk->model << "\n"
        << "Physical sector (size/count): " << disk->physical_sector_size << " / " << disk->physical_sector_cnt << "\n"
        << "Sector size (min/opt): " << disk->min_sector_size << "b / "
        << disk->optimal_sector_size << "b" << "\n"
        << "System partition: " << disk->devname << "\n"
        << "Partition table: " << disk->partition_table << std::endl;

    disk_draw_info info{};
    info.name = disk->model;
    info.description = str.str();
    info.partitions = &disk->partitions;
    info.uuid = disk->disk_id;
    return info;
}

std::vector<disk_draw_info> driveprv::get_drives_info() const noexcept
{
    std::vector<disk_draw_info> result;
    std::transform(disks_.begin(), disks_.end(), std::back_inserter(result),
                   [](const std::pair<std::string, diskctx*>& p)
                   { return build_disk_info(p.second); });
   return result;
}

driveprv::driveprv(const std::vector<diskctx*>& dsk) noexcept
{
    for(auto d : dsk)
        disks_.emplace(d->disk_id, d);
}

const diskctx* driveprv::get_disk(const std::string& uuid) const noexcept
{
    auto disk_i = disks_.find(uuid);
    if(disk_i == disks_.end())
        return nullptr;

    return disk_i->second;
}

} // providers