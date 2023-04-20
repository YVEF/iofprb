#ifndef IOFPRB_GPTTYPES_H
#define IOFPRB_GPTTYPES_H

#include <unordered_map>
#include <string>

const std::unordered_map<std::string, std::string> guidToPartTypeMap_ =
{
        {"00000000-0000-0000-0000-000000000000", "Unused entry"},
        {"024DEE41-33E7-11D3-9D69-0008C781F39F", "MBR partition scheme"},
        {"C12A7328-F81F-11D2-BA4B-00A0C93EC93B", "EFI System"},
        {"21686148-6449-6E6F-744E-656564454649", "BIOS boot"},
        {"D3BFE2DE-3DAF-11DF-BA40-E3A556D89593", "Intel Fast Flash (iFFS) partition (for Intel Rapid Start technology)"},
        {"F4019732-066E-4E12-8273-346C5641494F", "Sony boot"},
        {"BFBFAFE7-A34F-448A-9A5B-6213EB736C22", "Lenovo boot"},

        // Linux partitions
        {"0FC63DAF-8483-4772-8E79-3D69D8477DE4", "Linux filesystem data"},
        {"A19D880F-05FC-4D3B-A006-743F0F84911E", "RAID partition"},
        {"44479540-F297-41B2-9AF7-D131D5F0458A", "Root partition (x86)"},
        {"4F68BCE3-E8CD-4DB1-96E7-FBCAF984B709", "Root partition (x86-64)"},
        {"69DAD710-2CE4-4E3C-B16C-21A1D49ABED3", "Root partition (32-bit ARM)"},
        {"B921B045-1DF0-41C3-AF44-4C6F280D3FAE", "Root partition (64-bit ARM/AArch64)"},
        {"BC13C2FF-59E6-4262-A352-B275FD6F7172", "/boot partition"},
        {"0657FD6D-A4AB-43C4-84E5-0933C84B4F4F", "Swap partition"},
        {"E6D6D379-F507-44C2-A23C-238F2A3DF928", "Logical Volume Manager (LVM)"},
        {"933AC7E1-2EB4-4F13-B844-0E14E2AEF915", "/home partition"},
        {"3B8F8425-20E0-4F3B-907F-1A25A76F98E8", "/srv (server data) partition"},
        {"7FFEC5C9-2D00-49B7-8941-3EA10A5586B7", "Plain dm-crypt partition"},
        {"CA7D7CCB-63ED-4C53-861C-1742536059CC", "LUKS partition"},

        // Windows partitions
        {"E3C9E316-0B5C-4DB8-817D-F92DF00215AE", "Microsoft Reserved Partition (MSR)"},
        {"EBD0A0A2-B9E5-4433-87C0-68B6B72699C7", "Basic data partition"},
        {"5808C8AA-7E8F-42E0-85D2-E1E90434CFB3", "Logical Disk Manager (LDM) metadata"},
        {"AF9B60A0-1431-4F62-BC68-3311714A69AD", "Logical Disk Manager data"},
        {"DE94BBA4-06D1-4D40-A16A-BFD50179D6AC", "Windows Recovery Environment"},
        {"37AFFC90-EF7D-4E96-91C3-2D7AE055B174", "IBM General Parallel File System (GPFS)"},
        {"E75CAF8F-F680-4CEE-AFA3-B001E56EFC2D", "Storage Spaces partition"},
        {"558D43C5-A1AC-43C0-AAC8-D1472B2923D1", "Storage Replica partition"}
};

#endif //IOFPRB_GPTTYPES_H
