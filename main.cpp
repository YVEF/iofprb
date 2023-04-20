#include "ui/application.h"
#include "providers/driveprv.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <fcntl.h>
#include <sstream>
#include "utils/debug.h"
#include <blkid/blkid.h>
#include "disk/diskctx.h"
#include "fs/gpttypes.h"
#include "utils/hlp.h"
#include <cmath>
#include <sys/ioctl.h>
#include <linux/pci_regs.h>
#include <linux/serial_reg.h>
#include <linux/hdreg.h>
#include <cstring>
#include <libudev.h>
#include <sys/stat.h>
#include <algorithm>
#include <sys/statvfs.h>
#include <filesystem>
#include <sys/ioctl.h>
#include <sys/io.h>
#include <linux/fs.h>
#include <sys/mount.h>
#include "fs/mntpoint.h"
#include <linux/magic.h>
#include <unistd.h>
#include <sys/param.h>
#include "fs/swap.h"
#include "fs/fat.h"

// #include <sys/uio.h>

#define PROC_PARTITIONS "/proc/partitions"
#define PROC_MOUNTS "/proc/mounts"

void capture_blk_info(int fd, diskctx* disk, const std::vector<fs::mntpoint>& mount_points);
bool is_physical_partition(const std::string& partname);
void read_disk_model(int fd, diskctx* disk);
void fill_partition_space(partinfo* partition);
void probe_fs(int fd, partinfo* partition);

int main()
{
    std::string line;
    std::vector<fs::mntpoint> mount_points;
    std::ifstream mount_stream(PROC_MOUNTS);
    ASSERT(mount_stream);
    char bufdev[30], bufmnt[30], buffs[30];
    while(std::getline(mount_stream, line))
    {
        std::sscanf(line.c_str(), "%s %s %s ", bufdev, bufmnt, buffs);
        mount_points.emplace_back(fs::mntpoint{bufdev, bufmnt, buffs});
    }
    mount_stream.close();

    std::vector<diskctx*> disks;
    std::ifstream proc_stream(PROC_PARTITIONS);
    ASSERT(proc_stream);

    // header
    std::getline(proc_stream, line);
    // empty line
    std::getline(proc_stream, line);
    std::string part_name;
    while(std::getline(proc_stream, line))
    {
        part_name = "/dev/" + line.substr(line.rfind(' ') + 1);
        if(!is_physical_partition(part_name))
            continue;

        int fd = open(part_name.c_str(), O_RDONLY | O_CLOEXEC | O_NONBLOCK);
        ASSERT(fd);
        auto ctx = new diskctx(part_name);
        capture_blk_info(fd, ctx, mount_points);
        disks.push_back(ctx);
        close(fd);
    }
    proc_stream.close();

    // move sorting to diskprv
    std::sort(disks.begin(), disks.end(),
              [](const diskctx* d1, const diskctx* d2) { return d1->partitions.size() < d2->partitions.size(); });
    providers::driveprv drives(disks);


    ui::application app(drives);
    app.init();
    app.run();
    app.cleanup();

    return 0;
}

void capture_blk_info(int fd, diskctx* disk, const std::vector<fs::mntpoint>& mount_points)
{
    read_disk_model(fd, disk);

    disk->size = blkid_get_dev_size(fd);
    blkid_probe pr = blkid_new_probe();
    ASSERT(pr);
    ASSERT(!blkid_probe_set_device(pr, fd, 0, 0));

    blkid_topology tp = blkid_probe_get_topology(pr);
    ASSERT(tp);

    disk->min_sector_size = blkid_topology_get_minimum_io_size(tp);
    disk->optimal_sector_size = blkid_topology_get_optimal_io_size(tp);
    if(!disk->optimal_sector_size)
        disk->optimal_sector_size = disk->min_sector_size;

    disk->physical_sector_size = blkid_topology_get_physical_sector_size(tp);
    disk->logical_sector_size = blkid_topology_get_logical_sector_size(tp);
    disk->offset = blkid_topology_get_alignment_offset(tp);
    disk->physical_sector_cnt = disk->size / disk->physical_sector_size;
    disk->logical_sector_cnt = disk->size / disk->logical_sector_size;

    blkid_partlist ls = blkid_probe_get_partitions(pr);
    ASSERT(ls);
    int parnums = blkid_partlist_numof_partitions(ls);
    for(int i=0; i<parnums; i++)
    {
        blkid_partition par = blkid_partlist_get_partition(ls, i);
        ASSERT(par);

        // find more ways to grab the name
        auto partinfo = new struct partinfo(disk->name + std::to_string(blkid_partition_get_partno(par)),
                                            blkid_partition_is_logical(par),
                                            blkid_partition_is_primary(par));

        disk->partitions.push_back(partinfo);
        partinfo->sector_cnt = blkid_partition_get_size(par);

        // hold partition descriptor here
        int fdpart = open(partinfo->name.c_str(), O_RDONLY | O_NONBLOCK);
        partinfo->size = blkid_get_dev_size(fdpart);
        probe_swap_partition(fdpart, partinfo);
        // do not probe swap partition
        if(!partinfo->is_swap)
        {
            const std::string& devname = partinfo->name;
            auto mntp = std::find_if(mount_points.begin(), mount_points.end(),
                                     [&devname](const fs::mntpoint& p)
                                     { return p.dev == devname; });

            if(mntp == mount_points.end())
                // check the fs directly for not mounted partition
                probe_fs(fdpart, partinfo);
            else
            {
                partinfo->fstype = mntp->fstype;
                partinfo->mntroot = mntp->mntroot;
                fill_partition_space(partinfo);
            }
        }
        close(fdpart);

        partinfo->partition_type_guid = blkid_partition_get_type_string(par);
        to_ascii_upper_case(partinfo->partition_type_guid);

        partinfo->offset = blkid_partition_get_start(par);
        partinfo->flags = blkid_partition_get_flags(par);
        partinfo->uuid = blkid_partition_get_uuid(par);
        partinfo->type = blkid_partition_get_type(par);
        blkid_parttable table = blkid_partition_get_table(par);
        ASSERT(table);

        if(disk->partition_table.empty() && partinfo->is_primary)
        {
            disk->partition_table = blkid_parttable_get_type(table);
            to_ascii_upper_case(disk->partition_table);
        }
        if(disk->disk_id.empty() && partinfo->is_primary)
            disk->disk_id = blkid_parttable_get_id(table);

        auto prttbl = guidToPartTypeMap_.find(partinfo->partition_type_guid);
        ASSERT(prttbl != guidToPartTypeMap_.end());
        partinfo->partition_type = prttbl->second;
    }

    blkid_free_probe(pr);
}

bool is_physical_partition(const std::string& partname) { return !std::isdigit(partname[partname.size()-1]); }

void read_disk_model(int fd, diskctx* disk)
{
    // check for SATA drives
    struct hd_driveid drvid;
    if(!ioctl(fd, HDIO_GET_IDENTITY, &drvid))
    {
        disk->model = reinterpret_cast<const char*>(drvid.model);
        disk->serialno = reinterpret_cast<const char*>(drvid.serial_no);
        auto erase_cond = [](char ch) { return isdigit(ch) || isalpha(ch); };
        disk->model.erase(std::find_if(disk->model.rbegin(), disk->model.rend(), erase_cond).base(),
                          disk->model.end());
        disk->serialno.erase(std::find_if(disk->serialno.rbegin(), disk->serialno.rend(), erase_cond).base(),
                             disk->serialno.end());
        return;
    }

    // checking for usb drives
    struct udev_list_entry *devices, *dev_list_entry;

    struct udev* udev = udev_new();
    ASSERT(udev);
    struct udev_enumerate* enumerate = udev_enumerate_new(udev);
    ASSERT(enumerate);

    udev_enumerate_add_match_subsystem(enumerate, "block");
    udev_enumerate_add_match_property(enumerate, "DEVTYPE", "disk");
    udev_enumerate_add_match_property(enumerate, "DEVNAME", disk->name.c_str());
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;
        path = udev_list_entry_get_name(dev_list_entry);
        ASSERT(path);
        struct udev_device* dev = udev_device_new_from_syspath(udev, path);
        ASSERT(dev);

        if (udev_device_get_property_value(dev, "ID_BUS") &&
            strcmp(udev_device_get_property_value(dev, "ID_BUS"), "usb") == 0)
        {
            disk->model = udev_device_get_property_value(dev, "ID_MODEL");
            disk->serialno = udev_device_get_property_value(dev, "ID_SERIAL_SHORT");
        }

        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);
}

void fill_partition_space(partinfo* partition)
{
    struct statvfs64 stat;
    if(statvfs64(partition->mntroot.c_str(), &stat) < 0)
    {
        fprintf(stderr, "Couldn't read partition %s stat", partition->mntroot.c_str());
        return;
    }

    partition->free = stat.f_bfree*stat.f_bsize;
}

void probe_fs(int fd, partinfo* partition)
{
    if(fs::probe_fat_fs(fd, partition))
        return;

}