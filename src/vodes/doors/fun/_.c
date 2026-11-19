// Doors: opening, closing and where a swung door is
#define door_swing_angle (-0.5f * (float) M_PI)    // a quarter turn, its free edge to its back (+z), away from its placer
#define door_swing_time 0.4f                     // seconds to open or close

// its centre and turn swung (0 closed to 1 open, eased): turned about its hinge
static inline void door_swung(float3 rest, float4 rest_rotation, float3 hinge, float swing, float3* position, float4* rotation) {
    const float t = swing * swing * (3 - 2 * swing);
    *rotation = quaternion_rotate(rest_rotation, quaternion_from_axis_angle(float3_up, t * door_swing_angle));
    // the hinge stays: rest + R h - R' h
    *position = float3_add(rest, float3_sub(float4_rotate_float3(rest_rotation, hinge), float4_rotate_float3(*rotation, hinge)));
}

// a just spawned one (its components still deferred): set, not read
static inline void door_place(ecs* world, entity e, float3 rest, float4 rest_rotation, float3 hinge, float swing, float scale) {
    float3 position;
    float4 rotation;
    door_swung(rest, rest_rotation, hinge, swing, &position, &rotation);
    zox_setv(e, Position3D, position);
    zox_setv(e, Rotation3D, rotation);
    zox_setv(e, TransformMatrix, float4x4_transform_scale(position, rotation, scale));
}

// a door's block vox spawned (a hook of vodes_spawn_system): its rest, its hinge, swung as its block data says
//  - its model is two blocks a side from its base's corner, the door the base's x (0 to 1 block), thin about z 0.5:
//    the hinge is on its x 0 edge, a block from the centre in x and half a block in z
void door_on_spawned(ecs* world, spawned_block_data* data) {
    if (!zox_has(data->block, BlockDoor) || !data->octree || !is_linked_VoxelNode(data->octree)) {
        return;
    }
    const entity e = get_entity_VoxelNode(data->octree);
    const byte block_data = zox_has(data->chunk, BlockDatas) ? get_block_data(zox_get(data->chunk, BlockDatas), data->positionl) : 0;
    const byte open = (block_data & block_data_open) != 0;
    const float4 rest_rotation = block_direction_rotation(block_data_direction(block_data));
    const float3 hinge = { -data->scale, 0, -0.5f * data->scale };
    zox_setv(e, DoorOpen, open);
    zox_setv(e, DoorSwing, open);
    zox_setv(e, DoorRestPosition, data->positionf);
    zox_setv(e, DoorRestRotation, rest_rotation);
    zox_setv(e, DoorHinge, hinge);
    if (open) {
        door_place(world, e, data->positionf, rest_rotation, hinge, 1, 1);  // instanced block voxes are scale 1
    }
}

// a voxel's block data written straight to its chunk (not its queue: its voxel stays), the chunk saved as edited
static inline byte terrain_set_block_data(ecs* world, entity terrain, int3 block, byte data) {
    byte3 local;
    const entity chunk = terrain_chunk_local(world, terrain, block, &local);
    if (!chunk || !zox_has(chunk, BlockDatas)) {
        return 0;
    }
    spinlock* lock = zox_octree_lock(world, chunk);
    zox_octree_lock_begin(lock);
    zox_muter(chunk, BlockDatas, datas);
    set_block_data(datas, local, data);
    zox_octree_lock_end(lock);
    if (!zox_has(chunk, Edited)) {
        zox_add(chunk, Edited);
    }
    return 1;
}

// a voxel's block vox, 0 if none spawned
static inline entity terrain_vode_at(ecs* world, entity terrain, int3 block) {
    byte3 local;
    const entity chunk = terrain_chunk_local(world, terrain, block, &local);
    if (!chunk) {
        return 0;
    }
    const VoxelNode* node = get_VoxelNode(zox_get(chunk, VoxelNode), zox_getv(chunk, NodeDepth), local);
    return node && is_linked_VoxelNode(node) ? get_entity_VoxelNode(node) : 0;
}

// opens or closes the door with a part at block: every part's block data, its block vox swings and sounds
//  - from a worker too (block_activate_system): data under its chunks' locks, the rest deferred
byte door_toggle(ecs* world, entity terrain, entity realm, int3 block) {
    int3 base;
    const entity kind = multi_block_base_at(world, terrain, realm, block, &base);
    if (!kind || !zox_has(kind, BlockDoor)) {
        return 0;
    }
    const byte data = terrain_block_data_at(world, terrain, base);
    const byte open = !(data & block_data_open);
    const byte direction = block_data_direction(data);
    const int3 size = block_size_of(world, realm_voxel_block(world, realm, terrain_voxel_at(world, terrain, base)));
    const byte flag = open ? block_data_open : 0;
    for (int x = 0; x < size.x; x++)
    for (int y = 0; y < size.y; y++)
    for (int z = 0; z < size.z; z++) {
        const int3 o = block_direction_offset(direction, (int3) { x, y, z });
        const byte part = !x && !y && !z ? 1 + direction : block_offset_encode(int3_scale(o, -1));
        terrain_set_block_data(world, terrain, int3_add(base, o), part | flag);
    }
    const entity vode = terrain_vode_at(world, terrain, base);
    if (zox_valid(vode) && zox_has(vode, DoorOpen)) {
        zox_setv(vode, DoorOpen, open);
        zox_add(vode, DoorSound);
    }
    return 1;
}

// a door toggled (DoorSound): its creak opening, its thump closing, from where it hangs; on the main thread (spawns)
void on_door_sound(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        spawn_sound_effect_at(world, zox_getv(e, DoorOpen) ? sound_effect_door_open : sound_effect_door_close,
            zox_getv(e, DoorRestPosition));
        zox_remove(e, DoorSound);
    }
}
