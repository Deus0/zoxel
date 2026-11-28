// Lights block voxes (minivoxes) like the terrain around them
//  - the brightest of their own voxel and its 6 sides, sides past the chunk edge come from the neighbor chunk; one
//    that stops light (a shut door) the darkest of its open sides
//  - minivox blocks pass light (BlockLightPass), so their own voxel is lit; the sides cover blocks that don't
//  - brightness is light / 255, the same as terrain vertex colors (build_smooth_lights_system)

// a chunk's lights, voxels and block actives, its neighbors' and the realm's solidity, read once for its block voxes
typedef struct {
    const LightOctree* lights;
    const LightOctree* neighbor_lights[6];
    const LightOctree* block_lights;            // blocks' light (glow stones), the brighter of the two lights it
    const LightOctree* neighbor_block_lights[6];
    const VoxelNode* voxels;
    const VoxelNode* neighbor_voxels[6];
    const ActiveOctree* actives;
    const ActiveOctree* neighbor_actives[6];
    byte solidity[zox_light_solidity_length];
    byte depth;
} block_vox_lighting;

static inline byte block_vox_lighting_begin(ecs* world, entity chunk, block_vox_lighting* l) {
    if (!zox_lighting || !zox_valid(chunk) || !zox_has(chunk, LightOctree) || !zox_has(chunk, NodeDepth)) {
        return 0;
    }
    memset(l, 0, sizeof(block_vox_lighting));
    l->lights = zox_get(chunk, LightOctree);
    l->block_lights = chunk_lights(world, chunk, zox_light_block);
    l->depth = zox_getv(chunk, NodeDepth);
    l->voxels = zox_has(chunk, VoxelNode) ? zox_get(chunk, VoxelNode) : NULL;
    l->actives = zox_has(chunk, ActiveOctree) ? zox_get(chunk, ActiveOctree) : NULL;
    if (zox_has(chunk, ChunkNeighbors)) {
        const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
        fetch_neightbor_light_nodes(world, neighbors, l->neighbor_lights, zox_light_sky);
        fetch_neightbor_light_nodes(world, neighbors, l->neighbor_block_lights, zox_light_block);
        fetch_neightbor_voxel_nodes(world, neighbors, l->neighbor_voxels);
        fetch_neighbor_actives(world, neighbors, l->neighbor_actives);
    }
    entity cached = 0;
    update_light_solidity(world, zox_get_link(world, chunk, BlockManagerLink), &cached, l->solidity);
    return 1;
}

// does the voxel at position in a tree stop light (a shut door, its filler), no voxels: it passes
static inline byte block_vox_stops(const block_vox_lighting* l, const VoxelNode* voxels, const ActiveOctree* actives,
    byte3 position)
{
    return voxels && light_stops(l->solidity, actives, l->depth, getv_VoxelNode(voxels, l->depth, position), position);
}

// a voxel's light in a tree, 0 without one (a chunk without blocks' light)
static inline byte block_vox_light_at(const LightOctree* lights, byte depth, byte3 position) {
    return lights ? getv_LightOctree(lights, depth, position) : 0;
}

// a block vox's light: the brightest of its own voxel and its 6 sides; one that stops light (a shut door, the filler
//  over it) holds none of its own, so the darkest of its sides light passes through (a door between a sunlit outside
//  and a dark room lit as the room: lit by the brightest it glowed inside); sides past the chunk edge from its neighbor
static inline byte get_block_vox_light(const block_vox_lighting* l, byte3 position, byte channel) {
    const byte block = channel == zox_light_block;
    const LightOctree* own = block ? l->block_lights : l->lights;
    const byte shut = block_vox_stops(l, l->voxels, l->actives, position);
    byte light = shut ? 255 : block_vox_light_at(own, l->depth, position);
    byte found = !shut;
    const short length = octree_size(l->depth);
    for (byte d = 0; d < 6; d++) {
        int3 p = move_position(byte3_to_int3(position), d);
        const LightOctree* tree = own;
        const VoxelNode* voxels = l->voxels;
        const ActiveOctree* actives = l->actives;
        if (p.x < 0 || p.x >= length || p.y < 0 || p.y >= length || p.z < 0 || p.z >= length) {
            tree = block ? l->neighbor_block_lights[d] : l->neighbor_lights[d];
            voxels = l->neighbor_voxels[d];
            actives = l->neighbor_actives[d];
            if (!tree) {
                continue;
            }
            p = reverse_position(p, d, length);
        }
        const byte3 side = int3_to_byte3(p);
        const byte side_light = block_vox_light_at(tree, l->depth, side);
        if (!shut) {
            if (side_light > light) {
                light = side_light;
            }
        } else if (!block_vox_stops(l, voxels, actives, side)) {
            // a solid side (the wall round a door) holds darklight: only the open ones
            if (side_light < light) {
                light = side_light;
            }
            found = 1;
        }
    }
    return found ? light : darklight;
}

static inline float block_vox_light_to_brightness(byte light) {
    return light / 255.0f;
}

// a block vox's brightness at position in chunk in a channel's light (zox_light_sky its Brightness, zox_light_block
//  its BlockBrightness), the sky's full and no blocks' if the chunk has no lights
float get_block_vox_brightness(ecs* world, entity chunk, byte3 position, byte channel) {
    block_vox_lighting l;
    if (!block_vox_lighting_begin(world, chunk, &l)) {
        return channel == zox_light_block ? 0 : 1;
    }
    return block_vox_light_to_brightness(get_block_vox_light(&l, position, channel));
}

// a block vox at position in chunk lit: its Brightness (the sky's light) and BlockBrightness (blocks'), the sky's
//  full and no blocks' if the chunk has no lights
void set_block_vox_brightness(ecs* world, entity chunk, byte3 position, entity e) {
    block_vox_lighting l;
    const byte lit = block_vox_lighting_begin(world, chunk, &l);
    zox_setv(e, Brightness, lit ? block_vox_light_to_brightness(get_block_vox_light(&l, position, zox_light_sky)) : 1);
    zox_setv(e, BlockBrightness, lit ? block_vox_light_to_brightness(get_block_vox_light(&l, position, zox_light_block)) : 0);
}

// sets the Brightness and BlockBrightness of every block vox in chunk from its lights
void relight_block_voxes(ecs* world, entity chunk) {
    block_vox_lighting l;
    const byte lit = block_vox_lighting_begin(world, chunk, &l);
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
            const byte3 at = zox_getv(e2, VoxelLocalPosition);
            const float brightness = lit ? block_vox_light_to_brightness(get_block_vox_light(&l, at, zox_light_sky)) : 1;
            if (zox_getv(e2, Brightness) != brightness) {
                zox_setv(e2, Brightness, brightness);
            }
            const float glow = lit ? block_vox_light_to_brightness(get_block_vox_light(&l, at, zox_light_block)) : 0;
            if (zox_has(e2, BlockBrightness) && zox_getv(e2, BlockBrightness) != glow) {
                zox_setv(e2, BlockBrightness, glow);
            }
        }
    }
}
