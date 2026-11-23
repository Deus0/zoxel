// A chunk generated (new, or again finer for a lod) takes the light its neighbours hold at its borders
//  - its lights start dark (a lod's regeneration cleared them, terrain/sys/stream.c), and nothing else brings light
//    back in: a sunbeam comes down from the chunk above only as that one's lit, a flood stops at a chunk not there yet
//    (flood_light) and never comes back. A ground chunk regenerated under an air chunk that wasn't stayed dark for good:
//    its fine meshes drew dark, its coarse ones lit, lights flashed as lods swapped (tst chunk_mesh_flicker)
//  - from above: a sunbeam down each column whose bottom voxel in the chunk above is in full sunlight (light_beam_system
//    casts it at this chunk's depth)
//  - from each side: a flood from each border voxel of its neighbour, at that light less a step of air, at this chunk's
//    depth (the neighbour's voxel over it there)

// the light a neighbour holds at its border with this chunk, by this chunk's border voxel (u, v along the border at
//  depth): its voxel touching it at its own depth (a finer one's first of those, a coarser one's that holds it)
static inline byte chunk_relight_border_light(const LightOctree* lights, byte their_depth, byte depth, byte axis,
    byte positive, int u, int v)
{
    const byte a = axis == 0 ? 1 : 0, b = axis == 2 ? 1 : 2;
    const int their_length = octree_size(their_depth);
    int p[3];
    p[a] = their_depth >= depth ? u << (their_depth - depth) : u >> (depth - their_depth);
    p[b] = their_depth >= depth ? v << (their_depth - depth) : v >> (depth - their_depth);
    p[axis] = positive ? 0 : their_length - 1;
    return getv_LightOctree(lights, their_depth, (byte3) { p[0], p[1], p[2] });
}

void chunk_relight_from_neighbors(ecs* world, entity chunk) {
    if (zox_no_lights || !zox_has(chunk, LightQueue) || !zox_has(chunk, ChunkNeighbors)) {
        return;
    }
    const byte depth = zox_getv(chunk, NodeDepth);
    const int length = octree_size(depth);
    const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
    // sunlight from above: the chunk above's bottom row, at its own depth (the beam converts)
    const entity above = neighbors->value[direction_up];
    SunlightQueue* sunlight_queue = zox_get_mut(chunk, SunlightQueue);
    if (zox_valid(above) && zox_has(above, LightOctree)) {
        const LightOctree* lights = zox_get(above, LightOctree);
        const byte their_depth = zox_getv(above, NodeDepth);
        const int their_length = octree_size(their_depth);
        spin_lock(&sunlight_queue->lock);
        for (int x = 0; x < their_length; x++) {
            for (int z = 0; z < their_length; z++) {
                const byte light = getv_LightOctree(lights, their_depth, (byte3) { x, 0, z });
                if (light >= sunlight) {
                    a_SunlightQueue(sunlight_queue, (SunlightUpdate) {
                        .pos = (byte3) { x, their_length - 1, z },
                        .light = light,
                        .depth = their_depth
                    });
                }
            }
        }
        spin_unlock(&sunlight_queue->lock);
    }
    // floods from each side: its border voxels, this chunk's at its depth
    LightQueue* light_queue = zox_get_mut(chunk, LightQueue);
    for (byte d = 0; d < 6; d++) {
        const entity neighbor = neighbors->value[d];
        if (!zox_valid(neighbor) || !zox_has(neighbor, LightOctree) || !zox_has(neighbor, NodeDepth)) {
            continue;
        }
        const LightOctree* lights = zox_get(neighbor, LightOctree);
        const byte their_depth = zox_getv(neighbor, NodeDepth);
        const int3 dir = get_direction_int3(d);
        const byte axis = dir.x ? 0 : dir.y ? 1 : 2;
        const byte positive = (dir.x + dir.y + dir.z) > 0;
        spin_lock(&light_queue->lock);
        for (int u = 0; u < length; u++) {
            for (int v = 0; v < length; v++) {
                int mine[3];
                const byte a = axis == 0 ? 1 : 0, b = axis == 2 ? 1 : 2;
                mine[a] = u;
                mine[b] = v;
                mine[axis] = positive ? length - 1 : 0;
                const byte light = chunk_relight_border_light(lights, their_depth, depth, axis, positive, u, v);
                if (light <= darklight + light_air_decay) {
                    continue;
                }
                a_LightQueue(light_queue, (LightUpdate) {
                    .light = light - light_air_decay,
                    .pos = (byte3) { mine[0], mine[1], mine[2] },
                    .depth = depth,
                    .distance = light_propogation_distance
                });
            }
        }
        spin_unlock(&light_queue->lock);
    }
}
