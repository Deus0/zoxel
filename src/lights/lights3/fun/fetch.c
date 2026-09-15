void fetch_neightbor_voxel_nodes(
    ecs* world,
    const ChunkNeighbors* neighbors,
    const VoxelNode** nodes
) {
    for (int i = 0; i < 6; i++) {
        const entity e = neighbors->value[i];
        nodes[i] = zox_valid(e) && zox_has(e, VoxelNode) ? zox_get(e, VoxelNode) : NULL;
    }
}

void fetch_neightbor_light_nodes(
    ecs* world,
    const ChunkNeighbors* neighbors,
    const LightNode** nodes
) {
    for (int i = 0; i < 6; i++) {
        entity e = neighbors->value[i];
        nodes[i] = zox_valid(e) &&
            zox_has(e, LightNode) ?
                zox_get(e, LightNode) :
                NULL;
    }
}

void fetch_neightbor_propogation_queues(ecs* world,
    const ChunkNeighbors* neighbors,
    LightQueue** queues
) {
    for (int i = 0; i < 6; i++) {
        const entity e = neighbors->value[i];
        if (zox_valid(e)) {
            queues[i] = zox_gett_mut(e, LightQueue);
        } else {
            queues[i] = NULL;
        }
    }
}

void fetch_neighbors_sunlight_queues(
    ecs* world,
    const entity* neighbors,
    SunlightQueue** queues
) {
    for (int i = 0; i < 6; i++) {
        entity e = neighbors[i];
        if (zox_valid(e)) {
            queues[i] = zox_gett_mut(e, SunlightQueue);
        } else {
            queues[i] = NULL;
        }
    }
}

void fetch_neightbor_dark_queues(
    ecs* world,
    const ChunkNeighbors* neighbors,
    DarkQueue** queues
) {
    for (int i = 0; i < 6; i++) {
        entity e = neighbors->value[i];
        if (zox_valid(e)) {
            queues[i] = zox_gett_mut(e, DarkQueue);
        } else {
            queues[i] = NULL;
        }
    }
}

void fetch_neighbors_light_locks(
    ecs* world,
    const entity* neighbors,
    spinlock** locks
) {
    for (int i = 0; i < 6; i++) {
        entity e = neighbors[i];
        locks[i] = zox_valid(e) &&
            zox_has(e, LightLock) ?
                &(zox_mut(e, LightLock)->value) :
                NULL;
    }
}
