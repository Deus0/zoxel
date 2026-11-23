// tst: door_old_filler, a door whose top half is the plain filler (saves from before the door filler) stops light
//  while shut and lets it through open (lights3/fun/solidity.c light_stops: a filler passes light as its base does)
//  - an all air chunk (terrain tst air_chunk.c, no player): a sealed brick box, a tunnel two tall through it (x 2 to
//    6, y 4 and 5, z 7), a glow stone at its end (x 7), a shut door at x 5 with a plain filler, the far side at x 3
//  - its tunnel's light cleared (built round sunlit air the box keeps some) and the glow stone lit again: the far
//    side's upper voxel, beside the filler, dark; opened (door_toggle), lit through it
//  - run: bash bsh/tst.sh door_old_filler
#define zox_tst_dof_door 5
byte zox_tst_dof_stage = 0;
int zox_tst_dof_settled = 0;
byte zox_tst_dof_far[2];    // shut, open
byte zox_tst_dof_near[2];   // beside the glow stone

static inline void zox_tst_dof_relight(ecs* world, entity chunk) {
    const byte depth = zox_getv(chunk, NodeDepth);
    spinlock* lock = &zox_getm(chunk, LightLock)->value;
    spin_lock(lock);
    LightOctree* lights = zox_getm(chunk, LightOctree);
    for (byte x = 2; x <= 6; x++) for (byte y = 4; y <= 5; y++) {
        set_LightOctree(lights, depth, (byte3) { x, y, 7 }, darklight);
    }
    spin_unlock(lock);
    zox_add(chunk, GenerateEmitterLight);
}

byte zox_tst_door_old_filler_ready(ecs* world) {
    const entity chunk = zox_tst_streamed_air_chunk(world);
    if (!chunk) {
        return 0;
    }
    zox_tst_progress(zox_tst_dof_stage * 100 + zox_tst_dof_settled);
    const entity realm = zox_tst_world_realm;
    if (zox_tst_dof_stage == 0) {
        const byte bricks = zox_getv(zox_tst_air_block(world, zox_id(BlockBricks)), BlockIndex);
        const byte glow = zox_getv(zox_tst_air_block(world, zox_id(BlockGlowStone)), BlockIndex);
        const byte door = zox_getv(zox_tst_air_block(world, zox_id(BlockDoor)), BlockIndex);
        const byte filler = realm_filler_voxel(world, realm);
        for (byte x = 1; x <= 8; x++) for (byte y = 3; y <= 6; y++) for (byte z = 6; z <= 8; z++) {
            const byte tunnel = x >= 2 && x <= 6 && (y == 4 || y == 5) && z == 7;
            if (x == 7 && y == 5 && z == 7) {
                zox_tst_room_set(world, chunk, (byte3) { x, y, z }, glow, 0);
            } else if (tunnel && x == zox_tst_dof_door) {
                // its base facing 0 (block data 1 + direction), the plain filler over it pointing down to it
                zox_tst_room_set(world, chunk, (byte3) { x, y, z }, y == 4 ? door : filler,
                    y == 4 ? 1 : block_offset_encode((int3) { 0, -1, 0 }));
            } else if (!tunnel) {
                zox_tst_room_set(world, chunk, (byte3) { x, y, z }, bricks, 0);
            }
        }
        zox_tst_dof_stage = 1;
        return 0;
    }
    zox_tst_waiting = zox_tst_dof_stage == 1 ? "the box built" : zox_tst_dof_stage == 2 ? "the glow stone's light, the door shut" :
        "light through the open door";
    if (zox_has(chunk, GenerateEmitterLight) || !zox_tst_room_settled(world, chunk, &zox_tst_dof_settled)) {
        return 0;
    }
    zox_tst_dof_settled = 0;
    if (zox_tst_dof_stage == 1) {
        zox_tst_dof_relight(world, chunk);
        zox_tst_dof_stage = 2;
        return 0;
    }
    const byte s = zox_tst_dof_stage - 2;
    zox_tst_dof_far[s] = zox_tst_room_light(world, chunk, (byte3) { 3, 5, 7 });
    zox_tst_dof_near[s] = zox_tst_room_light(world, chunk, (byte3) { 6, 5, 7 });
    if (zox_tst_dof_stage == 3) {
        return 1;
    }
    const int3 c = zox_getv(chunk, ChunkPosition);
    const int length = octree_size(zox_getv(chunk, NodeDepth));
    zox_tst_check(door_toggle(world, zox_tst_world_terrain, realm, (int3) { c.x * length + zox_tst_dof_door, c.y * length + 4, c.z * length + 7 }),
        "toggling the door did nothing");
    zox_tst_dof_stage = 3;
    return 0;
}

void zox_tst_door_old_filler_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_dof_stage == 3, "never got to the end, at stage [%i]", zox_tst_dof_stage);
    zox_log("[tst] far side: shut [%i] open [%i], beside the glow stone [%i] [%i]",
        zox_tst_dof_far[0], zox_tst_dof_far[1], zox_tst_dof_near[0], zox_tst_dof_near[1]);
    zox_tst_check(zox_tst_dof_near[0] > darklight + 150, "beside the glow stone is [%i]", zox_tst_dof_near[0]);
    zox_tst_check(zox_tst_dof_far[0] <= darklight, "light through the shut door's plain filler [%i]", zox_tst_dof_far[0]);
    zox_tst_check(zox_tst_dof_far[1] > darklight + 40, "no light through the open door [%i]", zox_tst_dof_far[1]);
}
