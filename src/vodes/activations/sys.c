// NOTE: Uses the block its ray is on, in reach, the frame it right clicks: by the block's kind
//  - multithreaded: a door's toggle writes its chunks' block data under their locks, its sets are deferred, its sound
//    spawns on the main thread (DoorSound)
void block_activate_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TriggerActionB);
    zox_sys_in(RaycastVoxelData);
    zox_sys_in(RaycastRange);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TriggerActionB, trigger);
        zox_sys_i(RaycastVoxelData, ray);
        zox_sys_i(RaycastRange, range);
        if (trigger->value != zox_dirty_active) {
            continue;
        }
        if ((ray->result != rayhit_terrain && ray->result != rayhit_block_vox) || ray->distance > range->value ||
            !zox_valid(ray->chunk) || !zox_has(ray->chunk, ChunkPosition)) {
            continue;
        }
        const entity terrain = zox_get_parent(world, ray->chunk);
        const entity realm = zox_get_parent(world, terrain);
        const int3 block = chunk_block_position(world, ray->chunk, ray->positionl);
        door_toggle(world, terrain, realm, block);
    }
    zox_sys_on_end();
} zoxd_system(block_activate_system);
