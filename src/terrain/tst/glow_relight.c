// tst: glow_relight, a glow stone in a room relights after a hole in its ceiling is dug and covered, and when its
//  chunk is loaded (lights3/sys/dark.c dark_relight_emitter, lights3/sys/emitters.c)
//  - the player's game; under a sunlit surface, a sealed tunnel dug two below it, a glow stone at its end
//  - lit: beside the glow stone bright
//  - a hole dug down from the sky into the tunnel's other end: the sun comes in; covered: its dark floods the tunnel,
//    and the glow stone lights it back (the dark took its light, nothing lit it again)
//  - loaded: the tunnel's light cleared as a loaded chunk's is, its emitters lit (GenerateEmitterLight): bright again
//  - run: bash bsh/tst.sh glow_relight
#define zox_tst_room_tunnel 5   // tunnel voxels, x0 to x0 + 4, the glow stone at x0 + 5
byte zox_tst_relight_stage = 0;
int zox_tst_relight_settled = 0;
double zox_tst_relight_time = 0;
entity zox_tst_relight_chunk = 0;
byte3 zox_tst_relight_at;           // the tunnel's start, its top voxel (the surface over it at y + 2)
byte zox_tst_relight_top = 0;       // the surface voxel, put back to cover the hole
byte zox_tst_relight_lit[4][zox_tst_room_tunnel];  // lit, sun in, covered, loaded

// a box under a sunlit surface, all solid, inside a chunk: its corner at (x0, ty - height + 1, z), its tunnel's top
//  row ty, the surface at ty + 2 over x0 (sunlit above), rows ty + 1 solid over it all (a sealed ceiling)
//  - width: voxels along x past x0 (the tunnel and what ends it), height: the tunnel's rows
//  - sunlit: the surface over it under the sky (a hole dug there lets the sun in), else any solid box will do
static inline byte zox_tst_room_find(ecs* world, entity chunk, int width, int height, byte sunlit, byte3* at) {
    const byte depth = zox_getv(chunk, NodeDepth);
    const int length = octree_size(depth);
    const VoxelNode* voxels = zox_get(chunk, VoxelNode);
    const LightOctree* lights = zox_get(chunk, LightOctree);
    for (int x = 1; x + width + 1 < length; x++) for (int z = 1; z < length - 1; z++) {
        for (int y = length - 2; y >= height + 2; y--) {
            if (!getv_VoxelNode(voxels, depth, (byte3) { x, y, z })) {
                continue;
            }
            // the column's surface: sunlit over it
            if (sunlit && (getv_VoxelNode(voxels, depth, (byte3) { x, y + 1, z }) ||
                getv_LightOctree(lights, depth, (byte3) { x, y + 1, z }) != sunlight))
            {
                break;
            }
            const int ty = y - 2;
            byte solid = 1;
            for (int bx = x - 1; bx <= x + width && solid; bx++) for (int by = ty - height; by <= ty + 1 && solid; by++)
            for (int bz = z - 1; bz <= z + 1 && solid; bz++) {
                solid = getv_VoxelNode(voxels, depth, (byte3) { bx, by, bz }) != 0;
            }
            if (solid) {
                *at = (byte3) { x, ty, z };
                return 1;
            }
            if (sunlit) {
                break;  // the column's surface wasn't one
            }
        }
    }
    return 0;
}

// a chunk near the player at full detail with such a box, the nearest first (5 x 5 x 5 round it)
static inline entity zox_tst_room_chunk(ecs* world, int width, int height, byte sunlit, byte3* at) {
    const entity character = zox_tst_player_character(world);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const byte depth = zox_getv(terrain, NodeDepth);
    const int3 c = real_position_to_chunk_position(zox_getv(character, Position3D), octree_size(depth), zox_getv(terrain, BlockScale));
    for (int d = 0; d < 125; d++) {
        const int3 o = { d % 5 - 2, (d / 5) % 5 - 2, d / 25 - 2 };
        const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, int3_add(c, o));
        if (zox_valid(chunk) && zox_has(chunk, LightOctree) && zox_has(chunk, VoxelNodeQueue) &&
            zox_getv(chunk, NodeDepth) == depth && !zox_chunk_lighting_busy(world, chunk) &&
            zox_tst_room_find(world, chunk, width, height, sunlit, at))
        {
            return chunk;
        }
    }
    return 0;
}

static inline void zox_tst_room_set(ecs* world, entity chunk, byte3 position, byte value, byte data) {
    a_VoxelNodeQueue(zox_getm(chunk, VoxelNodeQueue), (VoxelNodeUpdate) { .value = value, .data = data, .position = position });
}

// its lights settled: nothing queued in it or its sides, its edits done, for 10 frames in a row
static inline byte zox_tst_room_settled(ecs* world, entity chunk, int* frames) {
    byte busy = zox_chunk_lighting_busy(world, chunk) || zox_get(chunk, VoxelNodeQueue)->count;
    const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
    for (byte d = 0; d < 6 && !busy; d++) {
        busy = zox_valid(neighbors->value[d]) && zox_has(neighbors->value[d], LightQueue) &&
            zox_chunk_lighting_busy(world, neighbors->value[d]);
    }
    *frames = busy ? 0 : *frames + 1;
    return *frames >= 10;
}

static inline byte zox_tst_room_light(ecs* world, entity chunk, byte3 position) {
    return getv_LightOctree(zox_get(chunk, LightOctree), zox_getv(chunk, NodeDepth), position);
}

static inline void zox_tst_relight_read(ecs* world, byte* lights) {
    for (byte i = 0; i < zox_tst_room_tunnel; i++) {
        const byte3 p = { zox_tst_relight_at.x + i, zox_tst_relight_at.y, zox_tst_relight_at.z };
        lights[i] = zox_tst_room_light(world, zox_tst_relight_chunk, p);
    }
}

byte zox_tst_glow_relight_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_progress(zox_tst_relight_stage * 100 + zox_tst_relight_settled);
    const entity chunk = zox_tst_relight_chunk;
    const byte3 at = zox_tst_relight_at;
    if (zox_tst_relight_stage == 0) {
        zox_tst_waiting = "a sealed room under sunlit ground near the player";
        zox_tst_relight_chunk = zox_tst_room_chunk(world, zox_tst_room_tunnel + 1, 1, 1, &zox_tst_relight_at);
        if (!zox_tst_relight_chunk) {
            return 0;
        }
        const entity character = zox_tst_player_character(world);
        const entity glow = zox_get_child_by_id(world, zox_get_link(world, character, RealmLink), zox_id(BlockGlowStone));
        const byte3 a = zox_tst_relight_at;
        zox_log("[tst] room at [%ix%ix%i]", a.x, a.y, a.z);
        for (byte i = 0; i < zox_tst_room_tunnel; i++) {
            zox_tst_room_set(world, zox_tst_relight_chunk, (byte3) { a.x + i, a.y, a.z }, 0, 0);
        }
        zox_tst_room_set(world, zox_tst_relight_chunk, (byte3) { a.x + zox_tst_room_tunnel, a.y, a.z }, zox_getv(glow, BlockIndex), 0);
        zox_tst_relight_stage = 1;
        return 0;
    }
    if (zox_tst_relight_stage == 1) {
        zox_tst_waiting = "the glow stone to light the room";
        if (!zox_tst_room_settled(world, chunk, &zox_tst_relight_settled)) {
            return 0;
        }
        zox_tst_relight_read(world, zox_tst_relight_lit[0]);
        // the hole: down from the sky into the tunnel's start
        zox_tst_relight_top = getv_VoxelNode(zox_get(chunk, VoxelNode), zox_getv(chunk, NodeDepth), (byte3) { at.x, at.y + 2, at.z });
        zox_tst_room_set(world, chunk, (byte3) { at.x, at.y + 2, at.z }, 0, 0);
        zox_tst_room_set(world, chunk, (byte3) { at.x, at.y + 1, at.z }, 0, 0);
        zox_tst_relight_settled = 0;
        zox_tst_relight_stage = 2;
        return 0;
    }
    if (zox_tst_relight_stage == 2) {
        zox_tst_waiting = "the sun to come in";
        if (!zox_tst_room_settled(world, chunk, &zox_tst_relight_settled)) {
            return 0;
        }
        zox_tst_relight_read(world, zox_tst_relight_lit[1]);
        zox_tst_room_set(world, chunk, (byte3) { at.x, at.y + 2, at.z }, zox_tst_relight_top, 0);
        zox_tst_relight_settled = 0;
        zox_tst_relight_stage = 3;
        return 0;
    }
    if (zox_tst_relight_stage == 3) {
        zox_tst_waiting = "the covered room to relight";
        if (!zox_tst_room_settled(world, chunk, &zox_tst_relight_settled)) {
            return 0;
        }
        zox_tst_relight_read(world, zox_tst_relight_lit[2]);
        // loaded: a loaded chunk's light starts dark, its emitters light it (the tunnel and the hole's shaft)
        spinlock* lock = &zox_getm(chunk, LightLock)->value;
        spin_lock(lock);
        LightOctree* lights = zox_getm(chunk, LightOctree);
        for (byte i = 0; i < zox_tst_room_tunnel; i++) {
            set_LightOctree(lights, zox_getv(chunk, NodeDepth), (byte3) { at.x + i, at.y, at.z }, darklight);
        }
        set_LightOctree(lights, zox_getv(chunk, NodeDepth), (byte3) { at.x, at.y + 1, at.z }, darklight);
        spin_unlock(lock);
        zox_add(chunk, GenerateEmitterLight);
        zox_tst_relight_settled = 0;
        zox_tst_relight_stage = 4;
        return 0;
    }
    zox_tst_waiting = "the loaded room's glow stone to light it";
    if (zox_has(chunk, GenerateEmitterLight) || !zox_tst_room_settled(world, chunk, &zox_tst_relight_settled)) {
        return 0;
    }
    zox_tst_relight_read(world, zox_tst_relight_lit[3]);
    return 1;
}

void zox_tst_glow_relight_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_relight_stage == 4, "never got to the end, at stage [%i]", zox_tst_relight_stage);
    const char* names[4] = { "lit", "sun in", "covered", "loaded" };
    for (byte s = 0; s < 4; s++) {
        const byte* l = zox_tst_relight_lit[s];
        zox_log("[tst] %s [%i %i %i %i %i]", names[s], l[0], l[1], l[2], l[3], l[4]);
    }
    const byte last = zox_tst_room_tunnel - 1;  // beside the glow stone
    zox_tst_check(zox_tst_relight_lit[0][last] > darklight + 150, "beside the glow stone is [%i]", zox_tst_relight_lit[0][last]);
    zox_tst_check(zox_tst_relight_lit[1][0] == sunlight, "the sun didn't come in the hole, [%i] under it", zox_tst_relight_lit[1][0]);
    for (byte s = 2; s < 4; s++) {
        int lost = 0;
        for (byte i = 0; i < zox_tst_room_tunnel; i++) {
            lost += zox_tst_relight_lit[s][i] + 4 < zox_tst_relight_lit[0][i];
        }
        zox_tst_check(!lost, "%s: [%i] tunnel voxels darker than the glow stone lit them", names[s], lost);
    }
    zox_tst_check(zox_tst_relight_lit[2][0] < sunlight, "the sun's still in the covered hole [%i]", zox_tst_relight_lit[2][0]);
}
