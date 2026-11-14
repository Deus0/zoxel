// a tunk's top down colours (TunkMapColors), made once its chunks are generated: in each column the first block from
//  the top, its colour shaded by its height, unflipped (x + z * side)
//  - returns 1 when it has them (made now or before), 0 while it or its chunks are still generating
//  - the maps (maps/sys/view.c) and map pieces (tunk_texture_system) read them, cleared when its chunks change
#define tunk_map_color_boost 0.5f
byte tunk_map_colors(ecs* world, entity tunk) {
    if (!zox_valid(tunk) || !zox_has(tunk, TunkMapColors) || !zox_has(tunk, Chunk3Stack)) {
        return 0;
    }
    if (zox_get(tunk, TunkMapColors)->length) {
        return 1;
    }
    if (!zox_has(tunk, GenerateTunk) || zox_getv(tunk, GenerateTunk)) {
        return 0;
    }
    const Chunk3Stack* stack = zox_get(tunk, Chunk3Stack);
    const VoxelNode* stackv[render_distance_y * 2 + 1];
    byte chunk_depth = 0;
    for (int k = render_distance_y * 2; k >= 0; k--) {
        const entity chunk = stack->value[k];
        stackv[k] = 0;
        if (!zox_valid(chunk)) {
            continue;
        }
        if (zox_has(chunk, GenerateChunk)) {
            return 0;
        }
        stackv[k] = zox_get(chunk, VoxelNode);
        chunk_depth = zox_getv(chunk, NodeDepth);
    }
    const entity terrain = zox_get_parent(world, tunk);
    const entity realm = zox_valid(terrain) ? zox_get_parent(world, terrain) : 0;
    if (!zox_valid(realm) || !zox_has(realm, BlockLinks)) {
        return 0;
    }
    const BlockLinks* blocks = zox_get(realm, BlockLinks);
    color block_colors[blocks->length];
    for (byte k = 0; k < blocks->length; k++) {
        block_colors[k] = zox_valid(blocks->value[k]) ? zox_getv(blocks->value[k], Color) : color_black;
    }
    const int side = powers_of_two[chunk_depth];
    const float height_div = render_distance_y * 2 * side;
    zox_muter(tunk, TunkMapColors, cache);
    resize_TunkMapColors(cache, side * side);
    byte3 position = byte3_zero;
    for (position.x = 0; position.x < side; position.x++) {
        for (position.z = 0; position.z < side; position.z++) {
            // from the top of the world, down to the first block
            byte top_voxel = 0;
            byte top_height = 255;
            for (int k = render_distance_y * 2; k >= 0 && !top_voxel; k--) {
                if (!stackv[k]) {
                    continue;
                }
                for (sbyte y = side - 1; y >= 0; y--) {
                    position.y = y;
                    const byte voxel = getv_VoxelNode(stackv[k], chunk_depth, position);
                    if (voxel) {
                        top_voxel = voxel;
                        top_height = k * side + y;
                        break;
                    }
                }
            }
            const color block_color = top_voxel && top_voxel <= blocks->length ? block_colors[top_voxel - 1] : color_black;
            cache->value[position.x + position.z * side] =
                color_multiply_float(block_color, top_height / height_div + tunk_map_color_boost);
        }
    }
    return 1;
}

// a chunk's voxels changed (VoxelNodeDirty): its tunk's colours are made again, its terrain's maps redraw
void tunk_map_colors_dirty(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain) || !zox_has(terrain, TunkLinks) || !zox_has(e, ChunkPosition)) {
            continue;
        }
        const int3 position = zox_getv(e, ChunkPosition);
        const entity tunk = int2_hashmap_get(zox_get(terrain, TunkLinks)->value, (int2) { position.x, position.z });
        if (!zox_valid(tunk) || !zox_has(tunk, TunkMapColors) || !zox_get(tunk, TunkMapColors)->length) {
            continue;
        }
        zox_muter(tunk, TunkMapColors, cache);
        resize_TunkMapColors(cache, 0);
        const int version = zox_has(terrain, TunkMapsVersion) ? zox_getv(terrain, TunkMapsVersion) : 0;
        zox_setv(terrain, TunkMapsVersion, version + 1);
    }
}
