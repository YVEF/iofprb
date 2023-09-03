#ifndef IOFPRB_CONFIG_STATE_H
#define IOFPRB_CONFIG_STATE_H

#include "../ambient/driveprv.h"
#include "../utils/typedefs.h"
#include <vector>
#include <string>
#include <cstring>

#define CNFG_ITERATION_COUNT_LIM 16
#define CNFG_THREADS_CNT_LIM 19
#define CNFG_1KB 1024U
#define CNFG_1MG 1048576U
#define CNFG_HIST_DIR_LEN 256

enum class access_strategy
{
    SEQ,
    RND
};

enum class engine
{
    STRD,
    KMOD

#ifdef DUMMY_TEST_ENGINE
    ,TEST
#endif
};


struct config_state
{
    template<typename TFirst>
    using valuelist_pairs_t = std::vector<std::pair<TFirst, std::string_view>>;

    int partition_id = 0;
    int iterations_id = 4;
    int alloc_chnk_id = 5;
    int threads_id = 0;
    int disk_info_id = 1;
    int access_id = 0;
    int block_size_id = 6;
    int engine_id = 0;

    const valuelist_pairs_t<uint> alloc_chunks {
            std::make_pair(16*CNFG_1MG, "16MiB"),
            std::make_pair(32*CNFG_1MG, "32MiB"),
            std::make_pair(64*CNFG_1MG, "64MiB"),
            std::make_pair(128*CNFG_1MG, "128MiB"),
            std::make_pair(256*CNFG_1MG, "256MiB"),
            std::make_pair(512*CNFG_1MG, "512MiB"),
            std::make_pair(1024*CNFG_1MG, "1GiB"),
            std::make_pair(2048*CNFG_1MG, "2GiB"),
            std::make_pair(4096*CNFG_1MG, "4GiB")
    };

    const valuelist_pairs_t<access_strategy> access {
            std::make_pair(access_strategy::SEQ, "SEQ"),
            std::make_pair(access_strategy::RND, "RND")
    };

    const valuelist_pairs_t<uint> block_size {
            std::make_pair(1, "1B"),
            std::make_pair(CNFG_1KB, "1Kib"),
            std::make_pair(CNFG_1KB*4, "4Kib"),
            std::make_pair(CNFG_1KB*8, "8Kib"),
            std::make_pair(CNFG_1KB*16, "16Kib"),
            std::make_pair(CNFG_1KB*32, "32Kib"),
            std::make_pair(CNFG_1KB*1024, "1Mib"),
    };

    const valuelist_pairs_t<engine> engines {
#ifdef DUMMY_TEST_ENGINE
            std::make_pair(engine::TEST, "dummy"),
#endif
            std::make_pair(engine::STRD, "syscalls"),
            std::make_pair(engine::KMOD, "kmodule")
    };

    valuelist_pairs_t<uint> iterations;
    valuelist_pairs_t<uint> threads;
    std::vector<ambient::disk_draw_info> disks_info;

    // build separated disk names vector
    // to satisfy a common infrastructure
    valuelist_pairs_t<uint> disk_names;
    bool preserve_history = true;
    char hist_dir[CNFG_HIST_DIR_LEN];

    explicit config_state(std::vector<ambient::disk_draw_info> disksinfo) noexcept;
    void reset_partition() noexcept { partition_id = 0; }

    [[nodiscard]] uint get_iterations() const noexcept;
    [[nodiscard]] uint get_threads() const noexcept;
    [[nodiscard]] engine get_engine() const noexcept;
    [[nodiscard]] const std::string& get_disk_uuid() const noexcept;
    [[nodiscard]] uint get_block_size() const noexcept;
    [[nodiscard]] uint get_alloc_chunk() const noexcept;
    [[nodiscard]] const std::string& get_disk_description() const noexcept;
};

#endif //IOFPRB_CONFIG_STATE_H
