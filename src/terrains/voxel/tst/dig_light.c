// tst: dig_light, digging a block out of sunlit ground doesn't flash its new faces (the light flickered when a block
//  was destroyed: the rebuilt mesh showed a frame white, unlit, before its light was baked in)
//  - the player's game; a sunlit surface block near the player, solid on its 4 sides and under it, removed
//  - each frame after (as uploaded: a mesh still building or uploading keeps the last frame's): the faces lining the hole (their middles on its cell's box) in the chunk's meshes (under its
//    materials), their
//    vertex colours' brightness (the light baked in)
//  - from when they all show: no frame more than 15% off the brightness they settle at (a flash); the faces around
//    it (within 4 voxels) stay within their brightness before and after, 15% either way
//  - run: bash bsh/tst.sh dig_light
#define zox_tst_dig_frames 60
byte zox_tst_dig_stage = 0;
int zox_tst_dig_frame = 0;
entity zox_tst_dig_chunk = 0;
byte3 zox_tst_dig_at;
int zox_tst_dig_faces[zox_tst_dig_frames];
float zox_tst_dig_bright[zox_tst_dig_frames];
float zox_tst_dig_around[zox_tst_dig_frames];
float zox_tst_dig_around_before = 0;
float zox_tst_dig_scale = 0;   // a voxel's size, the chunk is at full detail (the terrain's BlockScale)

// a sunlit surface voxel with solid on 4 sides and under, not at the chunk's edges, 0 if none
static inline byte zox_tst_dig_find(ecs* world, entity chunk, byte3* at) {
    const byte depth = zox_getv(chunk, NodeDepth);
    const int length = octree_size(depth);
    const VoxelNode* voxels = zox_get(chunk, VoxelNode);
    const LightOctree* lights = zox_get(chunk, LightOctree);
    // zox_tst_dig_edge: on the chunk's edge (x 0), its neighbour's faces and light rebuild too
    const byte edge = getenv("zox_tst_dig_edge") != NULL;
    for (int x = edge ? 0 : 2; x < (edge ? 1 : length - 2); x++) for (int z = 2; z < length - 2; z++) for (int y = length - 3; y >= 2; y--) {
        #define zox_tst_dig_v(dx, dy, dz) getv_VoxelNode(voxels, depth, (byte3) { x + dx, y + dy, z + dz })
        if (!zox_tst_dig_v(0, 0, 0) || zox_tst_dig_v(0, 1, 0)) {
            continue;
        }
        if (zox_tst_dig_v(1, 0, 0) && (edge || zox_tst_dig_v(-1, 0, 0)) && zox_tst_dig_v(0, 0, 1) && zox_tst_dig_v(0, 0, -1) &&
            zox_tst_dig_v(0, -1, 0) && getv_LightOctree(lights, depth, (byte3) { x, y + 1, z }) == sunlight)
        {
            *at = (byte3) { x, y, z };
            return 1;
        }
        #undef zox_tst_dig_v
        break;  // the column's surface wasn't one
    }
    return 0;
}

// the faces lining the hole in the chunk's meshes: their count, and their vertices' mean brightness (0 to 255)
// and the faces around it (their middles within 4 voxels of its), their mean brightness in around
static inline int zox_tst_dig_measure(ecs* world, entity chunk, byte3 at, float* brightness, float* around) {
    const float s = zox_tst_dig_scale;
    const float3 low = { at.x * s, at.y * s, at.z * s }, high = { (at.x + 1) * s, (at.y + 1) * s, (at.z + 1) * s };
    const float e = s * 0.01f;
    // its meshes: under its materials (chunk materials, a mesh each, water's apart)
    entity children[64];
    uint count = 0;
    entity materials[16];
    const uint material_count = zox_get_children(world, chunk, materials, 16);
    for (uint m = 0; m < material_count; m++) {
        count += zox_get_children(world, materials[m], children + count, 64 - count);
    }
    int faces = 0, around_faces = 0;
    double sum = 0, around_sum = 0;
    const float3 middle_at = float3_scale(float3_add(low, high), 0.5f);
    for (uint c = 0; c < count; c++) {
        const entity mesh = children[c];
        if (!zox_has(mesh, MeshVertices) || !zox_has(mesh, MeshColors)) {
            continue;
        }
        const MeshVertices* vertices = zox_get(mesh, MeshVertices);
        const MeshColors* colors = zox_get(mesh, MeshColors);
        if (colors->length != vertices->length) {
            continue;
        }
        // still building or uploading: the gpu has its last mesh, what's on screen didn't change (-1)
        if (zox_has(mesh, BuildMesh) || zox_has(mesh, BuildMeshColors) || zox_has(mesh, MeshDirty)) {
            return -1;
        }
        for (int i = 0; i + 3 < vertices->length; i += 4) {
            float3 middle = float3_zero;
            for (int k = 0; k < 4; k++) {
                float3_add_float3_p(&middle, vertices->value[i + k]);
            }
            middle = float3_scale(middle, 0.25f);
            if (float3_distance(middle, middle_at) <= 4 * s) {
                around_faces++;
                for (int k = 0; k < 4; k++) {
                    const color col = colors->value[i + k];
                    around_sum += col.r > col.g ? col.r : col.g;     // the brighter of the sky's (r) and blocks' (g)
                }
            }
            if (middle.x < low.x - e || middle.y < low.y - e || middle.z < low.z - e ||
                middle.x > high.x + e || middle.y > high.y + e || middle.z > high.z + e)
            {
                continue;
            }
            faces++;
            for (int k = 0; k < 4; k++) {
                const color col = colors->value[i + k];
                sum += col.r > col.g ? col.r : col.g;
            }
        }
    }
    *brightness = faces ? (float) (sum / (faces * 4)) : 0;
    *around = around_faces ? (float) (around_sum / (around_faces * 4)) : 0;
    return faces;
}

byte zox_tst_dig_light_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_progress(zox_tst_dig_stage * 1000 + zox_tst_dig_frame);
    if (zox_tst_dig_stage == 0) {
        zox_tst_waiting = "sunlit ground near the player";
        const entity character = zox_tst_player_character(world);
        const entity terrain = zox_get_link(world, character, TerrainLink);
        const byte depth = zox_getv(terrain, NodeDepth);
        zox_tst_dig_scale = zox_getv(terrain, BlockScale);
        const int3 c = real_position_to_chunk_position(zox_getv(character, Position3D), octree_size(depth),
            zox_getv(terrain, BlockScale));
        for (int d = 0; d < 27 && !zox_tst_dig_chunk; d++) {
            const int3 o = { d % 3 - 1, (d / 3) % 3 - 1, d / 9 - 1 };
            const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, int3_add(c, o));
            if (zox_valid(chunk) && zox_has(chunk, LightOctree) && zox_getv(chunk, NodeDepth) == depth &&
                zox_tst_dig_find(world, chunk, &zox_tst_dig_at))
            {
                zox_tst_dig_chunk = chunk;
            }
        }
        if (!zox_tst_dig_chunk) {
            return 0;
        }
        float bright;
        zox_log("[tst] digging [%ix%ix%i], faces there before [%i]", zox_tst_dig_at.x, zox_tst_dig_at.y, zox_tst_dig_at.z,
            zox_tst_dig_measure(world, zox_tst_dig_chunk, zox_tst_dig_at, &bright, &zox_tst_dig_around_before));
        a_VoxelNodeQueue(zox_getm(zox_tst_dig_chunk, VoxelNodeQueue), (VoxelNodeUpdate) { .value = 0, .position = zox_tst_dig_at });
        zox_tst_dig_stage = 1;
        return 0;
    }
    zox_tst_waiting = "the hole's faces over a second";
    const int f = zox_tst_dig_frame;
    zox_tst_dig_faces[f] = zox_tst_dig_measure(world, zox_tst_dig_chunk, zox_tst_dig_at, &zox_tst_dig_bright[f], &zox_tst_dig_around[f]);
    if (zox_tst_dig_faces[f] < 0) {
        zox_tst_dig_faces[f] = f ? zox_tst_dig_faces[f - 1] : 0;
        zox_tst_dig_bright[f] = f ? zox_tst_dig_bright[f - 1] : 0;
        zox_tst_dig_around[f] = f ? zox_tst_dig_around[f - 1] : zox_tst_dig_around_before;
    }
    return ++zox_tst_dig_frame >= zox_tst_dig_frames;
}

void zox_tst_dig_light_check(ecs* world) {
    const float settled = zox_tst_dig_bright[zox_tst_dig_frames - 1];
    const int all = zox_tst_dig_faces[zox_tst_dig_frames - 1];
    int dark = 0, first = -1;
    for (int f = 0; f < zox_tst_dig_frames; f++) {
        if (zox_tst_dig_faces[f] == all && first < 0) {
            first = f;
        }
        const float around_low = float_min(zox_tst_dig_around_before, zox_tst_dig_around[zox_tst_dig_frames - 1]) * 0.85f;
        const float around_high = float_max(zox_tst_dig_around_before, zox_tst_dig_around[zox_tst_dig_frames - 1]) * 1.15f;
        const byte darker = (first >= 0 && fabsf(zox_tst_dig_bright[f] - settled) > settled * 0.15f) ||
            zox_tst_dig_around[f] < around_low || zox_tst_dig_around[f] > around_high;
        dark += darker;
        if (f < 12 || darker) {
            zox_log("[tst] frame [%i] hole faces [%i] brightness [%.0f] around [%.0f]%s", f, zox_tst_dig_faces[f], zox_tst_dig_bright[f], zox_tst_dig_around[f], darker ? " FLASH" : "");
        }
    }
    zox_log("[tst] [metric] dig_light faces show at frame [%i], settle at brightness [%.0f], around it [%.0f] to [%.0f], [%i] frames flash", first, settled, zox_tst_dig_around_before, zox_tst_dig_around[zox_tst_dig_frames - 1], dark);
    const int expected = getenv("zox_tst_dig_edge") ? 4 : 5;    // on the edge its fifth is the neighbour chunk's
    zox_tst_check(first >= 0 && all >= expected, "the hole's faces never showed (or not [%i] of them: [%i])", expected, all);
    zox_tst_check(!dark, "the hole's faces were off the light they settle at for [%i] frames (a flash)", dark);
}
