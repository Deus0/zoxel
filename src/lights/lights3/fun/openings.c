// A voxel that starts or stops passing light without an edit: its voxel stays, its block data changes (a door's
//  parts opened or shut in its ActiveOctree, vodes/doors door_toggle). Its state is set first, then these run
//  - from a worker too: each queue and the light read are locked

// opened: its sides' light floods into it, and on through it (a queued flood starts at a voxel's own light)
void light_voxel_opened(ecs* world, entity chunk, byte3 position) {
    if (zox_no_lights || !zox_valid(chunk) || !zox_has(chunk, LightQueue) || !zox_has(chunk, ChunkNeighbors)) {
        return;
    }
    const byte depth = zox_getv(chunk, NodeDepth);
    const int length = octree_size(depth);
    LightQueue* sides[6];
    fetch_neightbor_propogation_queues(world, zox_get(chunk, ChunkNeighbors), sides);
    for (byte channel = zox_light_sky; channel <= zox_light_block; channel++) {
        for (byte d = 0; d < 6; d++) {
            const sbyte3 o = neighbor_offsets[d];
            emitter_queue_light(zox_getm(chunk, LightQueue), sides,
                (int3) { position.x + o.x, position.y + o.y, position.z + o.z }, length,
                (LightUpdate) { .light = darklight, .depth = depth, .distance = light_propogation_distance, .channel = channel });
        }
    }
}

// shut: its light goes, and the light that came through it (dark floods out from it, what's lit another way refills)
void light_voxel_shut(ecs* world, entity chunk, byte3 position) {
    if (zox_no_lights || !zox_valid(chunk) || !zox_has(chunk, DarkQueue) || !zox_has(chunk, LightOctree)) {
        return;
    }
    const byte depth = zox_getv(chunk, NodeDepth);
    spinlock* lock = &zox_getm(chunk, LightLock)->value;
    for (byte channel = zox_light_sky; channel <= zox_light_block; channel++) {
        spin_lock(lock);
        const byte light = getv_LightOctree(chunk_lights(world, chunk, channel), depth, position);
        spin_unlock(lock);
        if (light <= darklight) {
            continue;
        }
        emitter_queue_dark(zox_getm(chunk, DarkQueue), NULL, byte3_to_int3(position), octree_size(depth), (DarkUpdate) {
            .type = zox_light_type_flood,
            .depth = depth,
            .light = light,
            .distance = darklight_propogation_distance,
            .channel = channel
        });
    }
}
