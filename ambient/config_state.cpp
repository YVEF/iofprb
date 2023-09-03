#include "config_state.h"

const std::string& config_state::get_disk_description() const noexcept
{
    return disks_info[disk_info_id].description;
}

uint config_state::get_iterations() const noexcept
{
    return iterations[iterations_id].first;
}

uint config_state::get_threads() const noexcept
{
    return threads[threads_id].first;
}

engine config_state::get_engine() const noexcept
{
    return engines[engine_id].first;
}

const std::string& config_state::get_disk_uuid() const noexcept
{
    return disks_info[disk_info_id].uuid;
}

uint config_state::get_block_size() const noexcept
{
    return block_size[block_size_id].first;
}

uint config_state::get_alloc_chunk() const noexcept
{
    return alloc_chunks[alloc_chnk_id].first;
}

config_state::config_state(std::vector<ambient::disk_draw_info> disksinfo) noexcept
: disks_info(std::move(disksinfo))
{
    disk_names.reserve(disks_info.size());
    for(const auto& d : disks_info)
        disk_names.emplace_back(0, d.name);

    for(uint i=1; i < CNFG_ITERATION_COUNT_LIM; i++)
        iterations.emplace_back(i, std::to_string(i));

    for(uint i=1; i < CNFG_THREADS_CNT_LIM; i++)
        threads.emplace_back(i, std::to_string(i));

    // default hist path
    std::strcpy(hist_dir, "~/");
}


