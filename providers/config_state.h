#ifndef IOFPRB_CONFIG_STATE_H
#define IOFPRB_CONFIG_STATE_H

#include "../providers/driveprv.h"
#include "../utils/typedefs.h"
#include <vector>
#include <string>

#define CNFG_ITERATION_COUNT_LIM 16
#define CNFG_THREADS_CNT_LIM 19

enum class access_strategy
{
    SEQ,
    RND
};

enum class engine
{
    STRD,
    AHCI

#ifdef DUMMY_TEST_ENGINE
    ,TEST
#endif
};


struct config_state
{
    template<typename TFirst>
    using vector_pairs_t = std::vector<std::pair<TFirst, std::string_view>>;

    // defaults
    int partition_id = 0;
    int iterations_id = 4;
    int alloc_chnk_id = 5;
    int threads_id = 0;
    int disk_info_id = 0;
    int access_id = 0;
    int block_size_id = 2;
    int engine_id = 0;

    const vector_pairs_t<uint> alloc_chunks {
            std::make_pair(16, "16MiB"),
            std::make_pair(32, "32MiB"),
            std::make_pair(64, "64MiB"),
            std::make_pair(128, "128MiB"),
            std::make_pair(256, "256MiB"),
            std::make_pair(512, "512MiB"),
            std::make_pair(1024, "1GiB"),
            std::make_pair(2048, "2GiB"),
            std::make_pair(4096, "4GiB")
    };

    const vector_pairs_t<access_strategy> access {
            std::make_pair(access_strategy::SEQ, "SEQ"),
            std::make_pair(access_strategy::RND, "RND")
    };

    const vector_pairs_t<int> block_size {
            std::make_pair(1, "1B"),
            std::make_pair(1024, "1Kib"),
            std::make_pair(1024*4, "4Kib"),
            std::make_pair(1024*8, "8Kib"),
            std::make_pair(1024*16, "16Kib"),
            std::make_pair(1024*32, "32Kib"),
            std::make_pair(1024*1024, "1Mib"),
    };

    const vector_pairs_t<engine> engines {
#ifdef DUMMY_TEST_ENGINE
            std::make_pair(engine::TEST, "dummy"),
#endif
            std::make_pair(engine::STRD, "syscalls"),
            std::make_pair(engine::AHCI, "AHCI driver")
    };

    std::vector<uint> iterations;
    std::vector<int> threads;
    std::vector<providers::disk_draw_info> disks_info;

    explicit config_state(std::vector<providers::disk_draw_info> disksinfo) noexcept
            : disks_info(std::move(disksinfo))
    {
        for(uint i=1; i < CNFG_ITERATION_COUNT_LIM; i++)
            iterations.push_back(i);

        for(int i=1; i < CNFG_THREADS_CNT_LIM; i++)
            threads.push_back(i);
    }

    void to_default() noexcept
    {
        engine_id = 0;
        access_id = 0;
        block_size_id = 2;
        partition_id = 0;
        iterations_id = 4;
        alloc_chnk_id = 5;
        threads_id = 0;
    }
};

#endif //IOFPRB_CONFIG_STATE_H
