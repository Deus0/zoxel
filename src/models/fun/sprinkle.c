// Sprinkles: count seeded dots on a model's surface, within a box (position, size: from the corner, voxels)
//  - raised (zox_model_node_sprinkle): sets air touching the surface, bits sitting on it (crumbs, sprinkles)
//  - not raised (zox_model_node_speckle): paints solid voxels touching air, spots in it (chocolate chips)
//  - the same seed, shape and box give the same dots; a box with fewer places than count fills them all
//  - dot: each dot's side in voxels, dots snapped to a grid of it (so a dot is one voxel of a coarser depth,
//      and stays when the model's drawn at that lod); a dot fills only air (raised) or paints only solid (speckle)
//  - the surface is face neighbours (6), the octree's edge counts as air
//  - returns how many were set (tst model_nodes)

static inline byte voctree_sprinkle_solid(VoxelNode* voctree, byte depth, int length, int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x >= length || y >= length || z >= length) {
        return 0;
    }
    return getv_VoxelNode(voctree, depth, (byte3) { x, y, z }) != 0;
}

static inline byte voctree_sprinkle_touches(VoxelNode* voctree, byte depth, int length, int x, int y, int z, byte solid) {
    const int3 sides[6] = { { 1, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 }, { 0, 0, 1 }, { 0, 0, -1 } };
    for (byte i = 0; i < 6; i++) {
        if (voctree_sprinkle_solid(voctree, depth, length, x + sides[i].x, y + sides[i].y, z + sides[i].z) == solid) {
            return 1;
        }
    }
    return 0;
}

static inline uint voctree_sprinkle_rand(uint* state) {
    uint x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

int voctree_sprinkle(
    VoxelNode* voctree,
    byte depth,
    byte value,
    byte3 position,
    byte3 size,
    int count,
    uint seed,
    byte raised,
    int dot)
{
    if (dot < 1) {
        dot = 1;
    }
    const int length = octree_size(depth);
    const int3 end = {
        int_min(position.x + size.x, length),
        int_min(position.y + size.y, length),
        int_min(position.z + size.z, length) };
    if (count <= 0 || end.x <= position.x || end.y <= position.y || end.z <= position.z) {
        return 0;
    }
    // every place a dot could go, in order, then a seeded shuffle picks count of them
    const int volume = (end.x - position.x) * (end.y - position.y) * (end.z - position.z);
    byte3* places = malloc(sizeof(byte3) * volume);
    if (!places) {
        return 0;
    }
    int places_count = 0;
    for (int x = position.x; x < end.x; x++) {
        for (int y = position.y; y < end.y; y++) {
            for (int z = position.z; z < end.z; z++) {
                const byte solid = voctree_sprinkle_solid(voctree, depth, length, x, y, z);
                // raised: air on a solid; speckle: solid next to air
                if (raised ? (!solid && voctree_sprinkle_touches(voctree, depth, length, x, y, z, 1)) :
                    (solid && voctree_sprinkle_touches(voctree, depth, length, x, y, z, 0))) {
                    // snapped to the dot grid, once per cell (places are in order, a cell's come together or not:
                    //  so check the ones found so far for it, from the end, stopping past its x column)
                    const byte3 cell = { x - x % dot, y - y % dot, z - z % dot };
                    byte found = 0;
                    for (int k = places_count - 1; k >= 0 && places[k].x == cell.x; k--) {
                        if (places[k].y == cell.y && places[k].z == cell.z) {
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        places[places_count++] = cell;
                    }
                }
            }
        }
    }
    uint state = seed ? seed : 1;
    state ^= 0x9e3779b9;
    const int picks = count < places_count ? count : places_count;
    for (int i = 0; i < picks; i++) {
        const int j = i + (int) (voctree_sprinkle_rand(&state) % (uint) (places_count - i));
        const byte3 swap = places[i];
        places[i] = places[j];
        places[j] = swap;
        // the dot's cell: air filled (raised) or solid painted (speckle)
        for (int dx = 0; dx < dot; dx++) {
            for (int dy = 0; dy < dot; dy++) {
                for (int dz = 0; dz < dot; dz++) {
                    const int3 p = { places[i].x + dx, places[i].y + dy, places[i].z + dz };
                    if (voctree_sprinkle_solid(voctree, depth, length, p.x, p.y, p.z) != raised) {
                        set_voxel_safe(voctree, depth, (byte3) { p.x, p.y, p.z }, value);
                    }
                }
            }
        }
    }
    free(places);
    return picks;
}
