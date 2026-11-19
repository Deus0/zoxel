// a right click (TriggerActionB, set when pressed) on a door in reach toggles it
//  - an observer: the trigger is set once a press, it runs at the merge on the main thread (it writes chunks' data)
void on_door_activate(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TriggerActionB);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TriggerActionB, trigger);
        if (trigger->value != zox_dirty_trigger || !zox_has(e, RaycastVoxelData) || !zox_has(e, RaycastRange)) {
            continue;
        }
        const RaycastVoxelData* ray = zox_get(e, RaycastVoxelData);
        if ((ray->result != rayhit_terrain && ray->result != rayhit_block_vox) || ray->distance > zox_getv(e, RaycastRange) ||
            !zox_valid(ray->chunk) || !zox_has(ray->chunk, ChunkPosition)) {
            continue;
        }
        const entity terrain = zox_get_parent(world, ray->chunk);
        door_toggle(world, terrain, zox_get_parent(world, terrain), chunk_block_position(world, ray->chunk, ray->positionl), e);
    }
}

void zox_events_doors(ecs* world) {
    zox_observer(on_door_activate, EcsOnSet, [in] core.TriggerActionB);
}
