#include "config_state.h"


uint config_state::get_iterations() const noexcept
{
    return iterations[iterations_id];
}

uint config_state::get_threads() const noexcept
{
    return threads[threads_id];
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


