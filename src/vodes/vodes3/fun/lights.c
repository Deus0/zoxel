// Lights block voxes (minivoxes) like the terrain around them
//  - the brightest of their own voxel and its 6 sides, sides past the chunk edge come from the neighbor chunk
//  - minivox blocks pass light (BlockLightPass), so their own voxel is lit; the sides cover blocks that don't
//  - brightness is light / 255, the same as terrain vertex colors (build_smooth_lights_system)

static inline byte get_block_vox_light(
    const LightNode* lights,
    const LightNode** neighbor_lights,
    byte depth,
    byte3 position)
{
    byte light = getv_LightNode(lights, depth, position);
    const short length = octree_size(depth);
    for (byte d = 0; d < 6; d++) {
        int3 p = move_position(byte3_to_int3(position), d);
        const LightNode* tree = lights;
        if (p.x < 0 || p.x >= length || p.y < 0 || p.y >= length || p.z < 0 || p.z >= length) {
            tree = neighbor_lights[d];
            if (!tree) {
                continue;
            }
            p = reverse_position(p, d, length);
        }
        const byte side_light = getv_LightNode(tree, depth, int3_to_byte3(p));
        if (side_light > light) {
            light = side_light;
        }
    }
    return light;
}

static inline float block_vox_light_to_brightness(byte light) {
    return light / 255.0f;
}

// brightness of a block vox at position in chunk, 1 if the chunk has no lights
float get_block_vox_brightness(ecs* world, entity chunk, byte3 position) {
    if (zox_no_lights || !zox_valid(chunk) || !zox_has(chunk, LightNode) || !zox_has(chunk, NodeDepth)) {
        return 1;
    }
    const LightNode* neighbor_lights[6] = { 0 };
    if (zox_has(chunk, ChunkNeighbors)) {
        const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
        fetch_neightbor_light_nodes(world, neighbors, neighbor_lights);
    }
    const LightNode* lights = zox_get(chunk, LightNode);
    const byte light = get_block_vox_light(lights, neighbor_lights, zox_getv(chunk, NodeDepth), position);
    return block_vox_light_to_brightness(light);
}

// sets the Brightness of every block vox in chunk from its lights
void relight_block_voxes(ecs* world, entity chunk) {
    if (!zox_valid(chunk)) {
        return;
    }
    iter it2 = zox_children(world, chunk);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity e2 = it2.entities[j];
            if (!zox_has(e2, VoxelLocalPosition) || !zox_has(e2, Brightness)) {
                continue;
            }
            const float brightness = get_block_vox_brightness(world, chunk, zox_getv(e2, VoxelLocalPosition));
            if (zox_getv(e2, Brightness) != brightness) {
                zox_setv(e2, Brightness, brightness);
            }
        }
    }
}
