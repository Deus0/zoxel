// the terrain collision tests' own tiny terrain and bodies, turned so gravity points any way
//  - every test is written with gravity down (-y), in its local frame
//  - a frame turns that local frame into the world one (axes swapped and flipped): the terrain, the bodies' positions,
//      velocities, forces, gravity and boxes are all turned, so the same run happens against the turned terrain
//  - checks turn what they read back into the local frame (zox_tst_tc_local), so they're the same in every frame
//  - the terrain: a realm with one solid block, a terrain (block scale 0.5, chunks of 16), solid where a test's
//      zox_tst_tc_is_solid says (local block positions)

#define zox_tst_tc_depth 4           // 16 blocks a chunk
#define zox_tst_tc_scale 0.5f

// world axis i is sign[i] times local axis axis[i]
typedef struct {
    const char* name;
    byte axis[3];
    float sign[3];
} zox_tst_tc_frame;

#define zox_tst_tc_frame_down 0
#define zox_tst_tc_frame_up 1
#define zox_tst_tc_frame_x 2
#define zox_tst_tc_frame_z 3
const zox_tst_tc_frame zox_tst_tc_frames[] = {
    { "gravity -y", { 0, 1, 2 }, { 1, 1, 1 } },
    { "gravity +y", { 0, 1, 2 }, { 1, -1, 1 } },     // upside down: the floor is overhead
    { "gravity +x", { 1, 0, 2 }, { -1, 1, 1 } },     // local up is world -x, local x is world y
    { "gravity -z", { 0, 2, 1 }, { 1, 1, 1 } },      // local up is world +z
};
byte zox_tst_tc_frame_index = zox_tst_tc_frame_down;
void zox_tst_tc_frame_up_setup() { zox_tst_tc_frame_index = zox_tst_tc_frame_up; }
void zox_tst_tc_frame_x_setup() { zox_tst_tc_frame_index = zox_tst_tc_frame_x; }
void zox_tst_tc_frame_z_setup() { zox_tst_tc_frame_index = zox_tst_tc_frame_z; }

static inline float3 zox_tst_tc_world(const float3 local) {
    const zox_tst_tc_frame* f = &zox_tst_tc_frames[zox_tst_tc_frame_index];
    float3 world;
    for (byte i = 0; i < 3; i++) {
        *collision_axis_ptr(&world, i) = f->sign[i] * collision_axis_get(local, f->axis[i]);
    }
    return world;
}

static inline float3 zox_tst_tc_local(const float3 world) {
    const zox_tst_tc_frame* f = &zox_tst_tc_frames[zox_tst_tc_frame_index];
    float3 local;
    for (byte i = 0; i < 3; i++) {
        *collision_axis_ptr(&local, f->axis[i]) = f->sign[i] * collision_axis_get(world, i);
    }
    return local;
}

// half extents only swap
static inline float3 zox_tst_tc_world_half(const float3 local) {
    const zox_tst_tc_frame* f = &zox_tst_tc_frames[zox_tst_tc_frame_index];
    float3 world;
    for (byte i = 0; i < 3; i++) {
        *collision_axis_ptr(&world, i) = collision_axis_get(local, f->axis[i]);
    }
    return world;
}

// a flipped axis puts block k (k to k + 1) at -k - 1 (-k - 1 to -k), and the same for chunks
static inline int3 zox_tst_tc_local_block(const int3 world) {
    const zox_tst_tc_frame* f = &zox_tst_tc_frames[zox_tst_tc_frame_index];
    int3 local;
    for (byte i = 0; i < 3; i++) {
        const int w = collision_axis_get_int(world, i);
        *collision_axis_int(&local, f->axis[i]) = f->sign[i] > 0 ? w : -w - 1;
    }
    return local;
}

static inline int3 zox_tst_tc_world_chunk(const int3 local) {
    const zox_tst_tc_frame* f = &zox_tst_tc_frames[zox_tst_tc_frame_index];
    int3 world;
    for (byte i = 0; i < 3; i++) {
        const int l = collision_axis_get_int(local, f->axis[i]);
        *collision_axis_int(&world, i) = f->sign[i] > 0 ? l : -l - 1;
    }
    return world;
}

typedef byte (*zox_tst_tc_solid_fun)(const int3 block);

// a chunk at a local chunk position, solid where is_solid says
static inline entity zox_tst_tc_spawn_chunk(ecs* world, int3_hashmap* chunks, const int3 local_chunk, zox_tst_tc_solid_fun is_solid) {
    const int3 chunk_position = zox_tst_tc_world_chunk(local_chunk);
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
        if (is_solid(zox_tst_tc_local_block(block))) {
            set_VoxelNode(node, zox_tst_tc_depth, (byte3) { x, y, z }, 1);
        }
    }
    reduce_VoxelNode(node);
    int3_hashmap_add(chunks, chunk_position, chunk);
    return chunk;
}

// the terrain, its chunks at these local chunk positions
static inline entity zox_tst_tc_spawn_terrain(ecs* world, const int3* local_chunks, const int count, zox_tst_tc_solid_fun is_solid) {
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
    for (int i = 0; i < count; i++) {
        zox_tst_tc_spawn_chunk(world, chunk_links.value, local_chunks[i], is_solid);
    }
    return terrain;
}

typedef struct {
    const char* label;
    float3 position;
    float3 velocity;
    byte frictioned;
    float3 force;       // added every frame (a controller's push), below top speed along x if top is set
    float3 half;        // half extents, zero for zox_tst_tc_half
    float top;          // top speed along x for the force, 0 for none
    float jump;         // jumps at this speed within half a metre past jump_x (jump held), 0 for none: once grounded
    float jump_x;       //  for zox_tst_tc_jump_wait, as a character's CanJump waits out its landing (jumps/sys/jump_state.c)
    float stop_x;       // stops pushing past this x, 0 for never
    float grounded;     // how long it's been grounded, kept by zox_tst_tc_control
} zox_tst_tc_body;

#define zox_tst_tc_jump_wait 0.2f

const float3 zox_tst_tc_half = { 0.25f, 0.5f, 0.25f };

static inline float3 zox_tst_tc_body_half(const zox_tst_tc_body* b) {
    return b->half.x > 0 ? b->half : zox_tst_tc_half;
}

static inline entity zox_tst_tc_spawn_body(ecs* world, const entity terrain, const zox_tst_tc_body* b) {
    const entity e = zox_new();
    zox_set(e, Position3D, { zox_tst_tc_world(b->position) });
    zox_set(e, Rotation3D, { quaternion_identity });
    zox_set(e, Bounds3D, { zox_tst_tc_world_half(zox_tst_tc_body_half(b)) });
    zox_set(e, Velocity3D, { zox_tst_tc_world(b->velocity) });
    zox_set(e, Acceleration3D, { float3_zero });
    zox_set(e, Gravity3D, { zox_tst_tc_world(zox_default_gravity) });
    zox_set(e, Grounded, { 0 });
    zox_set_link(world, e, TerrainLink, terrain);
    if (b->frictioned) {
        zox_add(e, Frictioned);
    }
    return e;
}

// a frame's controls: the push, and the jump held
static inline void zox_tst_tc_control(ecs* world, const entity e, zox_tst_tc_body* b, const float dt) {
    const float3 p = zox_tst_tc_local(zox_getv(e, Position3D));
    float3 v = zox_tst_tc_local(zox_getv(e, Velocity3D));
    if (b->force.x != 0 || b->force.y != 0 || b->force.z != 0) {
        if ((!b->top || v.x < b->top) && (!b->stop_x || p.x < b->stop_x)) {
            Acceleration3D* a = zox_getm(e, Acceleration3D);
            a->value = float3_add(a->value, zox_tst_tc_world(b->force));
        }
    }
    b->grounded = zox_getv(e, Grounded) == ground_state_grounded ? b->grounded + dt : 0;
    if (b->jump && p.x > b->jump_x && p.x < b->jump_x + 0.5f && b->grounded >= zox_tst_tc_jump_wait) {
        b->grounded = 0;
        v.y = b->jump;
        zox_setv(e, Velocity3D, zox_tst_tc_world(v));
    }
}
