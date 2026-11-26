// tst: water_splash, falling into water splashes, climbing out splashes small (src/terrains/voxel/collisions/eve/water_splash.c)
//  - in an all air chunk (air_chunk.c, no player): a 3 x 3 pool of water 3 deep, a brick floor and walls round it
//  - a bare body (physics, InsideBlock) dropped from 4 blocks over its middle: a splash going in (zox_water_splashes_in counts it)
//  - then lifted 2 blocks over its surface: another, small, coming out
//  - run: bash bsh/tst.sh water_splash
byte zox_tst_splash_stage = 0;
double zox_tst_splash_time = 0;
entity zox_tst_splash_chunk = 0;
int zox_tst_splash_in = 0, zox_tst_splash_out = 0;
float3 zox_tst_splash_drop;

// the pool, local to the chunk: water x z 6..8, y 5..7; bricks under it (y 4) and round it (x z 5..9, y 5..7)
static inline byte zox_tst_splash_pool_at(int x, int y, int z) {
    if (x < 5 || x > 9 || z < 5 || z > 9 || y < 4 || y > 7) {
        return 0;
    }
    if (y == 4 || x == 5 || x == 9 || z == 5 || z == 9) {
        return 1;
    }
    return 2;
}

entity zox_tst_splash_body = 0;

// a bare body (no character): it falls (physics3), knows the block it's in (inside.c), splashes (water_splash.c)
static inline entity zox_tst_splash_spawn_body(ecs* world, entity terrain, float3 position) {
    const float scale = zox_getv(terrain, BlockScale);
    const entity e = zox_new();
    zox_set(e, Position3D, { position });
    zox_set(e, Rotation3D, { quaternion_identity });
    zox_set(e, Bounds3D, { float3_scale((float3) { 0.4f, 0.9f, 0.4f }, scale) });
    zox_set(e, Velocity3D, { float3_zero });
    zox_set(e, Acceleration3D, { float3_zero });
    zox_set(e, Gravity3D, { zox_default_gravity });
    zox_set(e, Grounded, { 0 });
    zox_set(e, InsideBlock, { 0 });
    zox_set(e, InsideBlockPosition, { int3_zero });
    zox_set(e, InsideBlockLast, { 0 });
    zox_set_link(world, e, TerrainLink, terrain);
    return e;
}

byte water_splash_ready(ecs* world) {
    const entity air = zox_tst_streamed_air_chunk(world);
    if (!air) {
        return 0;
    }
    const entity terrain = zox_tst_world_terrain;
    const entity realm = zox_tst_world_realm;
    const entity body = zox_tst_splash_body;
    zox_tst_progress(zox_tst_splash_stage * 1000 + (int) (zox_current_time - zox_tst_splash_time));
    if (zox_tst_splash_stage == 0) {
        zox_tst_splash_chunk = air;
        const entity water = zox_get_child_by_id(world, realm, zox_id(BlockWater));
        const entity bricks = zox_get_child_by_id(world, realm, zox_id(BlockBricks));
        zox_tst_check(zox_valid(water) && zox_valid(bricks), "the realm has no water or bricks block");
        if (!zox_valid(water) || !zox_valid(bricks)) {
            return 1;
        }
        VoxelNodeQueue* queue = zox_getm(zox_tst_splash_chunk, VoxelNodeQueue);
        for (byte x = 5; x <= 9; x++) for (byte y = 4; y <= 7; y++) for (byte z = 5; z <= 9; z++) {
            const byte at = zox_tst_splash_pool_at(x, y, z);
            a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = zox_getv(at == 2 ? water : bricks, BlockIndex),
                .position = (byte3) { x, y, z } });
        }
        // 4 blocks over the water's middle (local 7, 7, 7)
        const int length = octree_size(zox_getv(terrain, NodeDepth));
        const float scale = zox_getv(terrain, BlockScale);
        const int3 chunk = zox_getv(zox_tst_splash_chunk, ChunkPosition);
        zox_tst_splash_drop = (float3) { (chunk.x * length + 7.5f) * scale, (chunk.y * length + 12) * scale, (chunk.z * length + 7.5f) * scale };
        zox_tst_splash_stage = 1;
        zox_tst_splash_time = zox_current_time;
        return 0;
    }
    if (zox_tst_splash_stage == 1) {
        zox_tst_waiting = "the pool to be built";
        if (zox_current_time - zox_tst_splash_time < 1 || zox_has(zox_tst_splash_chunk, ChunkUpdate)) {
            return zox_current_time - zox_tst_splash_time > 15;
        }
        zox_tst_splash_body = zox_tst_splash_spawn_body(world, terrain, zox_tst_splash_drop);
        zox_water_splash_watch = zox_tst_splash_body;
        zox_tst_splash_stage = 2;
        zox_tst_splash_time = zox_current_time;
        return 0;
    }
    if (zox_tst_splash_stage == 2) {
        zox_tst_waiting = "the body to fall in and splash";
        // a while to fall in and settle, then out
        if (zox_current_time - zox_tst_splash_time < 2.5) {
            return 0;
        }
        zox_tst_splash_in = zox_water_splashes_in;
        // straight up, 2 blocks over the surface (local y 10)
        const float scale = zox_getv(terrain, BlockScale);
        zox_setv(body, Position3D, float3_add(zox_tst_splash_drop, (float3) { 0, -2 * scale, 0 }));
        zox_setv(body, Velocity3D, float3_zero);
        zox_tst_splash_stage = 3;
        zox_tst_splash_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "a splash coming out";
    // 0.2s: out, but not yet fallen back in
    if (zox_current_time - zox_tst_splash_time < 0.2) {
        return 0;
    }
    zox_tst_splash_out = zox_water_splashes_out;
    return 1;
}

void water_splash_check(ecs* world) {
    zox_log("[tst] [metric] water_splash splashes going in [%i] coming out [%i]", zox_tst_splash_in, zox_tst_splash_out);
    zox_tst_check(zox_tst_splash_stage == 3, "the test stopped at stage [%i]", zox_tst_splash_stage);
    zox_tst_check(zox_tst_splash_in == 1, "[%i] splashes falling into the water, not one", zox_tst_splash_in);
    zox_tst_check(zox_tst_splash_out == 1, "[%i] splashes coming out of the water, not one", zox_tst_splash_out);
    if (zox_valid(zox_tst_splash_body)) {
        zox_delete(zox_tst_splash_body);
    }
}
