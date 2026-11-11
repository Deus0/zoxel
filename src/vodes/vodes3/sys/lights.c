// Relights block voxes (minivoxes) when a chunk's lights change
//  - its neighbors too, block voxes on their edge take light from this chunk
void block_vox_lights_system(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LightNodeDirty);
    zox_sys_in(ChunkNeighbors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LightNodeDirty, dirty);
        zox_sys_i(ChunkNeighbors, neighbors);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        relight_block_voxes(world, e);
        for (byte j = 0; j < chunk_neighbors_length; j++) {
            relight_block_voxes(world, neighbors->value[j]);
        }
    }
} zoxd_system(block_vox_lights_system);
