// tst: gravity_cube, a gravity cube switched on flips gravity round it (src/terrains/voxel/gravity), in an all air chunk
//  (air_chunk.c, no game, no player)
//  - a brick floor (y 3), cube A at (14, 6, 8), two blocks from the chunk's +x side (its sphere reaches the next
//    chunk), no way of its own (it pulls up), cube B at (14, 6, 11) pulling +x (its block data, as placed looking
//    +x), three from A (their spheres overlap); a body with TerrainGravity on the floor by A
//  - each used as a right click uses it (block_use): A on, its sphere (gravity_radius) pulls up in both chunks and
//    nowhere else, the body's Gravity3D flips and it rises; B on, its sphere pulls +x, the nearest cube's way where
//    they overlap (as near to both: either); B off,
//    A's sphere alone; A dug out while on (nothing else on: only its removal clears the next chunk), every field's
//    empty, the body's pulled down again
//  - and a cube placed looking along each of the 6 ways pulls that way (block_pointing)
//  - run: bash bsh/tst.sh gravity_cube
byte zox_tst_gc_stage = 0;
int zox_tst_gc_frames = 0;
double zox_tst_gc_time = 0;
entity zox_tst_gc_body = 0;
int3 zox_tst_gc_a, zox_tst_gc_b;
float zox_tst_gc_start_y = 0, zox_tst_gc_top_y = 0;
// what each step found: voxels with the wrong value (in the chunk and its +x neighbour), the body's pull
int zox_tst_gc_wrong[5];
float zox_tst_gc_pull[5];
int zox_tst_gc_up[5];   // voxels pulling up
#define zox_tst_gc_b_pull 2     // +x (block_pointing)

static inline byte zox_tst_gc_in(int3 block, int3 cube) {
    const int3 d = int3_sub(block, cube);
    return d.x * d.x + d.y * d.y + d.z * d.z <= gravity_radius * gravity_radius;
}

static inline int zox_tst_gc_d2(int3 block, int3 cube) {
    const int3 d = int3_sub(block, cube);
    return d.x * d.x + d.y * d.y + d.z * d.z;
}

// voxels in the chunk at chunk_position whose field isn't what the cubes on (a, b) give
static inline int zox_tst_gc_check_chunk(ecs* world, entity terrain, int3 chunk_position, byte a, byte b, int* up) {
    const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, chunk_position);
    if (!zox_valid(chunk) || !zox_has(chunk, GravityOctree)) {
        return -1;
    }
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    const GravityOctree* field = zox_get(chunk, GravityOctree);
    int wrong = 0;
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        const int3 block = { chunk_position.x * length + x, chunk_position.y * length + y, chunk_position.z * length + z };
        const byte in_a = a && zox_tst_gc_in(block, zox_tst_gc_a), in_b = b && zox_tst_gc_in(block, zox_tst_gc_b);
        const int da = zox_tst_gc_d2(block, zox_tst_gc_a), db = zox_tst_gc_d2(block, zox_tst_gc_b);
        const byte value = getv_GravityOctree(field, depth, (byte3) { x, y, z });
        if (value == gravity_up) {
            (*up)++;
        }
        byte expected = gravity_default;
        if (in_a && (!in_b || da < db)) {
            expected = gravity_up;
        } else if (in_b && (!in_a || db < da)) {
            expected = zox_tst_gc_b_pull;
        } else if (in_a && in_b) {
            expected = value == gravity_up || value == zox_tst_gc_b_pull ? value : gravity_up;   // as near: either
        }
        if (value != expected) {
            wrong++;
        }
    }
    return wrong;
}

static inline void zox_tst_gc_record(ecs* world, entity terrain, entity chunk, int step, byte a, byte b) {
    const int3 c = zox_getv(chunk, ChunkPosition);
    zox_tst_gc_up[step] = 0;
    const int w0 = zox_tst_gc_check_chunk(world, terrain, c, a, b, &zox_tst_gc_up[step]);
    const int w1 = zox_tst_gc_check_chunk(world, terrain, int3_add(c, (int3) { 1, 0, 0 }), a, b, &zox_tst_gc_up[step]);
    zox_tst_gc_wrong[step] = w0 < 0 || w1 < 0 ? -1 : w0 + w1;
    zox_tst_gc_pull[step] = zox_getv(zox_tst_gc_body, Gravity3D).y;
}

byte zox_tst_gravity_cube_ready(ecs* world) {
    const entity chunk = zox_tst_streamed_air_chunk(world);
    if (!chunk) {
        return 0;
    }
    const entity terrain = zox_tst_world_terrain;
    const entity realm = zox_tst_world_realm;
    const int length = octree_size(zox_getv(terrain, NodeDepth));
    const int3 c = zox_getv(chunk, ChunkPosition);
    zox_tst_gc_frames++;
    if (zox_tst_gc_stage == 0) {
        const byte bricks = zox_getv(zox_tst_air_block(world, zox_id(BlockBricks)), BlockIndex);
        const byte cube = zox_getv(zox_tst_air_block(world, zox_id(BlockGravity)), BlockIndex);
        VoxelNodeQueue* queue = zox_getm(chunk, VoxelNodeQueue);
        for (byte x = 1; x <= 14; x++) for (byte z = 1; z <= 14; z++) {
            a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = bricks, .position = (byte3) { x, 3, z } });
        }
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = cube, .position = (byte3) { 14, 6, 8 } });
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = cube, .data = block_pointing((float3) { 1, 0, 0 }),
            .position = (byte3) { 14, 6, 11 } });
        zox_tst_gc_a = (int3) { c.x * length + 14, c.y * length + 6, c.z * length + 8 };
        zox_tst_gc_b = (int3) { c.x * length + 14, c.y * length + 6, c.z * length + 11 };
        zox_tst_gc_stage = 1;
        zox_tst_gc_time = zox_current_time;
        return 0;
    }
    if (zox_tst_gc_stage == 1) {
        zox_tst_waiting = "the floor and cubes to be built";
        if (zox_current_time - zox_tst_gc_time < 1 || zox_has(chunk, ChunkUpdate)) {
            return 0;
        }
        // a body on the floor by A, pulled by the field (as a character is)
        const entity e = zox_new();
        zox_set(e, Position3D, { zox_tst_air_position(world, chunk, (float3) { 11, 5, 8 }) });
        zox_set(e, Rotation3D, { quaternion_identity });
        zox_set(e, Bounds3D, { (float3) { 0.2f, 0.45f, 0.2f } });
        zox_set(e, Velocity3D, { float3_zero });
        zox_set(e, Acceleration3D, { float3_zero });
        zox_set(e, Gravity3D, { zox_default_gravity });
        zox_set(e, Grounded, { 0 });
        zox_add(e, TerrainGravity);
        zox_set_link(world, e, TerrainLink, terrain);
        zox_tst_gc_body = e;
        zox_tst_gc_stage = 2;
        zox_tst_gc_time = zox_current_time;
        return 0;
    }
    const entity body = zox_tst_gc_body;
    const float y = zox_getv(body, Position3D).y;
    if (zox_tst_gc_stage == 2) {
        zox_tst_waiting = "the body to land on the floor";
        if (zox_getv(body, Grounded) != ground_state_grounded && zox_current_time - zox_tst_gc_time < 3) {
            return 0;
        }
        zox_tst_gc_record(world, terrain, chunk, 0, 0, 0);
        zox_tst_gc_start_y = zox_tst_gc_top_y = y;
        block_use(world, 0, terrain, realm, zox_tst_gc_a);
        zox_tst_gc_stage = 3;
        zox_tst_gc_frames = 0;
        zox_tst_gc_time = zox_current_time;
        return 0;
    }
    if (y > zox_tst_gc_top_y) {
        zox_tst_gc_top_y = y;
    }
    if (zox_tst_gc_stage == 3) {
        zox_tst_waiting = "the body to rise, A on";
        if (zox_tst_gc_frames == 3) {
            zox_tst_gc_record(world, terrain, chunk, 1, 1, 0);
        }
        if (zox_current_time - zox_tst_gc_time < 1.5) {
            return 0;
        }
        block_use(world, 0, terrain, realm, zox_tst_gc_b);
        zox_tst_gc_stage = 4;
        zox_tst_gc_frames = 0;
        return 0;
    }
    if (zox_tst_gc_stage == 4) {
        zox_tst_waiting = "A and B on";
        if (zox_tst_gc_frames < 3) {
            return 0;
        }
        zox_tst_gc_record(world, terrain, chunk, 2, 1, 1);
        block_use(world, 0, terrain, realm, zox_tst_gc_b);
        zox_tst_gc_stage = 5;
        zox_tst_gc_frames = 0;
        return 0;
    }
    if (zox_tst_gc_stage == 5) {
        zox_tst_waiting = "B off";
        if (zox_tst_gc_frames < 3) {
            return 0;
        }
        zox_tst_gc_record(world, terrain, chunk, 3, 1, 0);
        // A dug out while it's on
        byte3 local = { 14, 6, 8 };
        a_VoxelNodeQueue(zox_getm(chunk, VoxelNodeQueue), (VoxelNodeUpdate) { .value = 0, .position = local });
        zox_tst_gc_stage = 6;
        zox_tst_gc_frames = 0;
        return 0;
    }
    zox_tst_waiting = "A dug out";
    if (zox_tst_gc_frames < 6 || zox_has(chunk, ChunkUpdate)) {
        return zox_tst_gc_frames > 600;
    }
    zox_tst_gc_record(world, terrain, chunk, 4, 0, 0);
    zox_tst_gc_stage = 7;
    return 1;
}

void zox_tst_gravity_cube_check(ecs* world) {
    const char* steps[5] = { "before", "A on", "A and B on", "B off", "A dug out" };
    const float g = zox_default_gravity.y;
    for (int i = 0; i < 5; i++) {
        zox_log("[tst] gravity_cube [%s]: [%i] voxels pull up, [%i] wrong, the body's pull [%.1f]", steps[i],
            zox_tst_gc_up[i], zox_tst_gc_wrong[i], zox_tst_gc_pull[i]);
    }
    zox_tst_check(zox_tst_gc_stage == 7, "the test stopped at stage [%i]", zox_tst_gc_stage);
    // placed looking along each of the 6 ways, it pulls that way (block_pointing, gravity_pull)
    const float3 ways[6] = { { -1, 0, 0 }, { 1, 0, 0 }, { 0, -1, 0 }, { 0, 1, 0 }, { 0, 0, -1 }, { 0, 0, 1 } };
    for (byte i = 0; i < 6; i++) {
        const float3 look = float3_add(ways[i], (float3) { 0.1f, -0.2f, 0.15f });     // a little off the axis
        const float3 pull = float3_normalize(gravity_pull(block_pointing(look)));
        zox_tst_check(float3_dot(pull, ways[i]) > 0.99f, "placed looking [%.0f %.0f %.0f] it pulls [%.1f %.1f %.1f]",
            ways[i].x, ways[i].y, ways[i].z, pull.x, pull.y, pull.z);
    }
    for (int i = 0; i < 5; i++) {
        zox_tst_check(zox_tst_gc_wrong[i] == 0, "[%s] [%i] voxels' fields aren't the cubes' spheres (-1: no +x neighbour chunk)",
            steps[i], zox_tst_gc_wrong[i]);
    }
    zox_tst_check(zox_tst_gc_up[1] > 200, "[A on] only [%i] voxels pull up, its sphere has 257", zox_tst_gc_up[1]);
    zox_tst_check(zox_tst_gc_pull[0] == g && zox_tst_gc_pull[4] == g, "the body's pull wasn't the realm's [%.1f] before [%.1f] and after [%.1f]",
        g, zox_tst_gc_pull[0], zox_tst_gc_pull[4]);
    zox_tst_check(zox_tst_gc_pull[1] == -g, "[A on] the body's pull is [%.1f], not flipped [%.1f]", zox_tst_gc_pull[1], -g);
    const float scale = zox_getv(zox_tst_world_terrain, BlockScale);
    zox_tst_check(zox_tst_gc_top_y - zox_tst_gc_start_y > scale, "[A on] the body rose [%.2f] blocks, not a block",
        (zox_tst_gc_top_y - zox_tst_gc_start_y) / scale);
}
