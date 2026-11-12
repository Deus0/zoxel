// tst: terrain_collisions, bodies against voxel terrain (terrain_collision_system)
//  - builds its own tiny terrain, no streaming: a realm with one solid block, a terrain
//      (block scale 0.5, chunks of 16) with two chunks side by side (x -8 to 8, z 0 to 8)
//      - floor: block layers 0 to 3, its top at y = 2
//      - wall: x 6 to 6.5, y 2 to 6, the whole chunk deep, on the right chunk
//      - ceiling: x -8 to -6.5, y 6 to 7, on the left chunk
//  - bodies are 0.5 x 1 x 0.5 (half extents 0.25 0.5 0.25), standing on the floor at y = 2.5
//  - 3s at a fixed 60fps (10 and 30fps in the _10fps and _30fps variants), gravity on, no friction:
//      drop: falls from rest, lands and rests on the floor, Grounded
//      fast: falls at 60 m/s (a metre a frame, more than its half height), doesn't sink in
//      bullet: falls at 400 m/s, its first frame ends 1.7m under the 2m floor, clear of it
//          (so only the sweep can catch it, not the stuck-in-a-block fallback)
//      wall: runs into the wall at 80 m/s (1.3m a frame), stops at its face (Frictioned,
//          so it doesn't bounce back off the terrain)
//      seam: slides over the chunk seam at x = 0 and every block seam, never snags or bobs
//      edge: walks off the end of the terrain (z = 8), falls, Grounded goes leaving -> air
//      buried: spawned inside the floor, comes out on top
//      jump: jumps at 8 m/s, peaks at v^2 / 2g above the floor, lands again
//      thin wall: touching the wall, at 90 m/s its first frame ends clear past the 0.5m wall
//      ceiling: jumps at 40 m/s into a 1m slab overhead (y 6 to 7), never gets through or into
//          it (rising against gravity is where the swept move is guessed least well)
//  - run: bash bsh/tst.sh terrain_collisions

#define zox_tst_tc_seconds 3.0
#define zox_tst_tc_depth 4           // 16 blocks a chunk
#define zox_tst_tc_scale 0.5f
#define zox_tst_tc_floor_top 2.0f    // block layers 0 to 3
#define zox_tst_tc_wall_x 6.0f       // block 12
#define zox_tst_tc_terrain_end_z 8.0f

#define zox_tst_tc_drop 0
#define zox_tst_tc_fast 1
#define zox_tst_tc_bullet 2
#define zox_tst_tc_wall 3
#define zox_tst_tc_seam 4
#define zox_tst_tc_edge 5
#define zox_tst_tc_buried 6
#define zox_tst_tc_jump 7
#define zox_tst_tc_thin_wall 8
#define zox_tst_tc_ceiling 9
#define zox_tst_tc_count 10

typedef struct {
    const char* label;
    float3 position;
    float3 velocity;
    byte frictioned;
} zox_tst_tc_body;

const float3 zox_tst_tc_half = { 0.25f, 0.5f, 0.25f };
const float zox_tst_tc_rest_y = zox_tst_tc_floor_top + 0.5f;

zox_tst_clock zox_tst_tc_clock;
byte zox_tst_tc_spawned = 0;
entity zox_tst_tc_terrain = 0;
entity zox_tst_tc_entities[zox_tst_tc_count];
zox_tst_tc_body zox_tst_tc_bodies[zox_tst_tc_count] = {
    { "drop", { 2, 6, 4 }, { 0, 0, 0 } },
    { "fast", { 3, 7, 4 }, { 0, -60, 0 } },
    { "bullet", { 4, 5, 4 }, { 0, -400, 0 } },
    { "wall", { 2, 2.51f, 2 }, { 80, 0, 0 }, 1 },
    { "seam", { -3, 2.51f, 6 }, { 2, 0, 0 } },
    { "edge", { 1, 2.51f, 7 }, { 0, 0, 3 } },
    { "buried", { -2, 1.0f, 1 }, { 0, 0, 0 } },
    { "jump", { -5, 2.5f, 4 }, { 0, 8, 0 } },
    { "thin wall", { 5.7f, 2.51f, 5 }, { 90, 0, 0 }, 1 },
    { "ceiling", { -7, 2.5f, 4 }, { 0, 40, 0 } },
};
// tracked every frame
float zox_tst_tc_lowest[zox_tst_tc_count];     // lowest y once it first touched the floor
float zox_tst_tc_highest[zox_tst_tc_count];    // highest y
float zox_tst_tc_furthest_x[zox_tst_tc_count]; // furthest x
byte zox_tst_tc_landed[zox_tst_tc_count];
float zox_tst_tc_seam_x_error = 0;             // worst |x - (x0 + v t)| on the seam body
byte zox_tst_tc_edge_states[3];                // edge body: saw grounded, leaving, air (in that order)

static inline byte zox_tst_tc_is_solid(const int3 block) {
    if (block.y < 4) {
        return 1;  // floor
    }
    if (block.x >= -16 && block.x <= -13 && (block.y == 12 || block.y == 13)) {
        return 1;  // ceiling, x -8 to -6.5, y 6 to 7
    }
    return block.x == 12 && block.y < 12;  // wall
}

static inline entity zox_tst_tc_spawn_chunk(ecs* world, int3_hashmap* chunks, const int3 chunk_position) {
    const entity chunk = zox_new();
    zox_set(chunk, NodeDepth, { zox_tst_tc_depth });
    zox_add(chunk, VoxelNode);
    zox_muter(chunk, VoxelNode, node);
    create_VoxelNode(node);
    const int length = 1 << zox_tst_tc_depth;
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        const int3 block = {
            chunk_position.x * length + x,
            chunk_position.y * length + y,
            chunk_position.z * length + z };
        if (zox_tst_tc_is_solid(block)) {
            set_VoxelNode(node, zox_tst_tc_depth, (byte3) { x, y, z }, 1);
        }
    }
    reduce_VoxelNode(node);
    int3_hashmap_add(chunks, chunk_position, chunk);
    return chunk;
}

static inline void zox_tst_tc_spawn_terrain(ecs* world) {
    const entity block = zox_new();
    zox_set(block, BlockCollider, { zox_block_solid });
    const entity realm = zox_new();
    zox_add(realm, BlockLinks);
    zox_muter(realm, BlockLinks, blocks);
    add_to_BlockLinks(blocks, block);
    const entity terrain = zox_new();
    zox_set(terrain, BlockScale, { zox_tst_tc_scale });
    zox_set(terrain, NodeDepth, { zox_tst_tc_depth });
    ChunkLinks chunk_links = { .value = create_int3_hashmap(64) };
    zox_set_ptr(terrain, ChunkLinks, chunk_links);
    zox_link(world, terrain, RealmLink, realm);
    zox_tst_tc_spawn_chunk(world, chunk_links.value, (int3) { 0, 0, 0 });
    zox_tst_tc_spawn_chunk(world, chunk_links.value, (int3) { -1, 0, 0 });
    zox_tst_tc_terrain = terrain;
}

static inline entity zox_tst_tc_spawn_body(ecs* world, const zox_tst_tc_body* b) {
    const entity e = zox_new();
    zox_set(e, Position3D, { b->position });
    zox_set(e, Rotation3D, { quaternion_identity });
    zox_set(e, Bounds3D, { zox_tst_tc_half });
    zox_set(e, Velocity3D, { b->velocity });
    zox_set(e, Acceleration3D, { float3_zero });
    zox_set(e, Gravity3D, { zox_default_gravity });
    zox_set(e, Grounded, { 0 });
    zox_set_link(world, e, TerrainLink, zox_tst_tc_terrain);
    if (b->frictioned) {
        zox_add(e, Frictioned);
    }
    return e;
}

byte zox_tst_terrain_collisions_ready(ecs* world) {
    zox_tst_waiting = "bodies to land on the terrain";
    if (!zox_tst_tc_spawned) {
        if (!zox_tst_clock_prepare(&zox_tst_tc_clock, zox_tst_physics_dt)) {
            return 0;
        }
        zox_tst_tc_spawned = 1;
        zox_tst_tc_spawn_terrain(world);
        for (int i = 0; i < zox_tst_tc_count; i++) {
            zox_tst_tc_entities[i] = zox_tst_tc_spawn_body(world, &zox_tst_tc_bodies[i]);
            zox_tst_tc_lowest[i] = 1000;
            zox_tst_tc_highest[i] = -1000;
            zox_tst_tc_furthest_x[i] = -1000;
        }
        return 0;
    }
    zox_tst_clock_tick(&zox_tst_tc_clock);
    for (int i = 0; i < zox_tst_tc_count; i++) {
        const entity e = zox_tst_tc_entities[i];
        const float3 p = zox_getv(e, Position3D);
        if (p.x > zox_tst_tc_furthest_x[i]) {
            zox_tst_tc_furthest_x[i] = p.x;
        }
        if (p.y > zox_tst_tc_highest[i]) {
            zox_tst_tc_highest[i] = p.y;
        }
        if (zox_getv(e, Grounded) == ground_state_grounded) {
            zox_tst_tc_landed[i] = 1;
        }
        if (zox_tst_tc_landed[i] && p.y < zox_tst_tc_lowest[i]) {
            zox_tst_tc_lowest[i] = p.y;
        }
    }
    // seam: slides at its start speed (no friction), so x is known every frame
    {
        const zox_tst_tc_body b = zox_tst_tc_bodies[zox_tst_tc_seam];
        const float x = zox_getv(zox_tst_tc_entities[zox_tst_tc_seam], Position3D).x;
        const float error = fabsf(x - (b.position.x + b.velocity.x * (float) zox_tst_tc_clock.time));
        if (error > zox_tst_tc_seam_x_error) {
            zox_tst_tc_seam_x_error = error;
        }
    }
    // edge: grounded, then leaving, then in the air
    {
        const byte state = zox_getv(zox_tst_tc_entities[zox_tst_tc_edge], Grounded);
        if (state == ground_state_grounded) {
            zox_tst_tc_edge_states[0] = 1;
        } else if (state == ground_state_leaving && zox_tst_tc_edge_states[0]) {
            zox_tst_tc_edge_states[1] = 1;
        } else if (state == ground_state_air && zox_tst_tc_edge_states[1]) {
            zox_tst_tc_edge_states[2] = 1;
        }
    }
    return zox_tst_physics_done(zox_tst_tc_clock.time, zox_tst_tc_seconds);
}

void zox_tst_terrain_collisions_check(ecs* world) {
    zox_tst_clock_check(&zox_tst_tc_clock, "terrain_collisions");
    const float rest = zox_tst_tc_rest_y;
    const float g = -zox_default_gravity.y;
    for (int i = 0; i < zox_tst_tc_count; i++) {
        const char* label = zox_tst_tc_bodies[i].label;
        const entity e = zox_tst_tc_entities[i];
        const float3 p = zox_getv(e, Position3D);
        const float3 v = zox_getv(e, Velocity3D);
        zox_tst_check(zox_tst_float3_finite(p) && zox_tst_float3_finite(v), "[%s] went NaN or infinite", label);
        if (i == zox_tst_tc_edge) {
            continue;
        }
        // resting on the floor, still, Grounded
        zox_tst_check(fabsf(p.y - rest) < 0.01f, "[%s] ended at y [%f], expected resting on the floor at [%f]", label, p.y, rest);
        zox_tst_check(fabsf(v.y) < 0.2f, "[%s] still moving up or down at [%f] on the floor", label, v.y);
        zox_tst_check(zox_getv(e, Grounded) == ground_state_grounded, "[%s] not Grounded on the floor [%i]", label, zox_getv(e, Grounded));
        // once landed, never sinks into the floor
        if (i != zox_tst_tc_buried) {
            zox_tst_check(zox_tst_tc_lowest[i] > rest - 0.02f,
                "[%s] sank to y [%f] after landing, the floor's at [%f]", label, zox_tst_tc_lowest[i], rest);
        }
    }
    // walls: never went past the face, but got there (then bounced back, bounce_lost_force)
    const int walls[] = { zox_tst_tc_wall, zox_tst_tc_thin_wall };
    for (int w = 0; w < 2; w++) {
        const float x = zox_tst_tc_furthest_x[walls[w]];
        const float face = zox_tst_tc_wall_x - zox_tst_tc_half.x;
        const char* label = zox_tst_tc_bodies[walls[w]].label;
        zox_tst_check(x <= face + 0.01f, "[%s] reached x [%f], into or through the wall, its face is at [%f]", label, x, face);
        zox_tst_check(x > face - 0.05f, "[%s] only reached x [%f], stopped short of the wall face [%f]", label, x, face);
    }
    // ceiling: its head never went above the slab's underside
    {
        const float head = zox_tst_tc_highest[zox_tst_tc_ceiling] + zox_tst_tc_half.y;
        zox_tst_check(head <= 6.0f + 0.01f, "[ceiling] head reached y [%f], into or through the ceiling at [6]", head);
        zox_tst_check(head >= 6.0f - 0.05f, "[ceiling] head only reached y [%f], should have hit the ceiling at [6]", head);
    }
    // seam: no snags over the chunk and block seams
    zox_tst_check(zox_tst_tc_seam_x_error < 0.01f,
        "[seam] snagged, x was off by up to [%f] from sliding freely", zox_tst_tc_seam_x_error);
    zox_tst_check(zox_tst_tc_highest[zox_tst_tc_seam] < rest + 0.02f,
        "[seam] bobbed up to y [%f] sliding over the floor at [%f]", zox_tst_tc_highest[zox_tst_tc_seam], rest);
    // edge: fell off the end, and Grounded went grounded -> leaving -> air
    {
        const entity e = zox_tst_tc_entities[zox_tst_tc_edge];
        const float3 p = zox_getv(e, Position3D);
        zox_tst_check(p.z > zox_tst_tc_terrain_end_z && p.y < zox_tst_tc_floor_top - 1,
            "[edge] at [%f %f %f], should have walked off the end of the terrain and fallen", p.x, p.y, p.z);
        zox_tst_check(zox_tst_tc_edge_states[0] && zox_tst_tc_edge_states[1] && zox_tst_tc_edge_states[2],
            "[edge] Grounded went through grounded [%i] leaving [%i] air [%i]",
            zox_tst_tc_edge_states[0], zox_tst_tc_edge_states[1], zox_tst_tc_edge_states[2]);
        zox_tst_check(zox_getv(e, Grounded) == ground_state_air, "[edge] still Grounded [%i] in the air", zox_getv(e, Grounded));
    }
    // jump: peak height v^2 / 2g (exact steps, frames land within g dt^2 / 2 below the top)
    {
        const float v0 = zox_tst_tc_bodies[zox_tst_tc_jump].velocity.y;
        const float peak = rest + v0 * v0 / (2 * g);
        const float height = zox_tst_tc_highest[zox_tst_tc_jump];
        const float dt = (float) zox_tst_tc_clock.dt;
        zox_tst_check(height <= peak + 0.01f && height >= peak - g * dt * dt / 2 - 0.01f,
            "[jump] peaked at y [%f], expected v^2 / 2g above the floor [%f]", height, peak);
    }
}

void zox_tsts_terrain_collisions() {
    zox_tst_add_physics("terrain_collisions", zox_tst_terrain_collisions_ready, zox_tst_terrain_collisions_check, 40);
}
