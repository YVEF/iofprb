#ifndef IOFPRB_SWAP_H
#define IOFPRB_SWAP_H
#include <cstdint>
#include <unistd.h>
#include "../disk/partinfo.h"
#include "../utils/debug.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#define SWAP_MAGIC1 "SWAP-SPACE"
#define SWAP_MAGIC2 "SWAPSPACE2"

#define SWAPFS "[swap]"

union swap_header {
    struct {
        // char reserved[PAGE_SIZE - 10]; use offset here
        char magic[10];
    } magic;
    struct {
        char		bootbits[1024];
        uint32_t 	version;
        uint32_t	last_page;
        uint32_t	nr_badpages;
        unsigned char	sws_uuid[16];
        unsigned char	sws_volume[16];
        uint32_t	padding[117];
        uint32_t	badpages[1];
    } info;
};

void probe_swap_partition(int fd, partinfo* partition)
{
    ull page_size = static_cast<ull>(getpagesize());
    if(partition->size < page_size)
        // to small to be a swap
        return;

    void* buffer = mmap(nullptr, page_size, PROT_READ, MAP_PRIVATE, fd, 0);
    ASSERT(buffer != MAP_FAILED);

    swap_header* header = reinterpret_cast<swap_header*>(static_cast<char*>(buffer) + page_size - 10);
    if(memcmp(header->magic.magic, SWAP_MAGIC2, sizeof(swap_header::magic.magic)) == 0
    || memcmp(header->magic.magic, SWAP_MAGIC1, sizeof(swap_header::magic.magic)) == 0)
    {
        partition->is_swap = true;
        partition->fstype = SWAPFS;
    }

    ASSERT(munmap(buffer, page_size) != -1);
}

#endif //IOFPRB_SWAP_H
