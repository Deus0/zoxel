// tst: door_glow, a shut room with a glow stone: its door opened lets the sun in, shut it goes back to the glow's light
//  (lights3/sys/dark.c, lights3/sys/emitters.c)
//  - an all air chunk in the void (terrain tst air_chunk.c, no player), sunlit: a sealed brick room (inside x 2 to 7,
//    y 4 to 6, z 6 to 8), a shut door in its +z wall at x 4, a glow stone inside at its far corner (2, 4, 6)
//  - built: lit by the glow alone; opened (door_toggle): brighter by the door; shut: by the door darker again, and
//    every voxel inside as the glow lit it (was: the dark took the glow's light and nothing lit it again)
//  - run: bash bsh/tst.sh door_glow
#define zox_tst_dglow_door_x 4
byte zox_tst_dglow_stage = 0;
int zox_tst_dglow_settled = 0;
int3 zox_tst_dglow_base;
byte zox_tst_dglow_lit[3][6][3][3];     // built, open, shut: the room's inside

static inline byte zox_tst_dglow_interior(int x, int y, int z) {
    return x >= 2 && x <= 7 && y >= 4 && y <= 6 && z >= 6 && z <= 8;
}

static inline void zox_tst_dglow_read(ecs* world, entity chunk, byte s) {
    for (byte x = 0; x < 6; x++) for (byte y = 0; y < 3; y++) for (byte z = 0; z < 3; z++) {
        zox_tst_dglow_lit[s][x][y][z] = zox_tst_room_light(world, chunk, (byte3) { x + 2, y + 4, z + 6 });
    }
}

byte zox_tst_door_glow_ready(ecs* world) {
    const entity chunk = zox_tst_streamed_air_chunk(world);
    if (!chunk) {
        return 0;
    }
    zox_tst_progress(zox_tst_dglow_stage * 100 + zox_tst_dglow_settled);
    const entity realm = zox_tst_world_realm;
    if (zox_tst_dglow_stage == 0) {
        const byte bricks = zox_getv(zox_tst_air_block(world, zox_id(BlockBricks)), BlockIndex);
        const entity door = zox_tst_air_block(world, zox_id(BlockDoor));
        for (byte x = 1; x <= 8; x++) for (byte y = 3; y <= 7; y++) for (byte z = 5; z <= 9; z++) {
            if (zox_tst_dglow_interior(x, y, z)) {
                continue;
            }
            if (x == zox_tst_dglow_door_x && z == 9 && y == 4) {
                zox_tst_room_set(world, chunk, (byte3) { x, y, z }, zox_getv(door, BlockIndex), 1);
            } else if (x == zox_tst_dglow_door_x && z == 9 && y == 5) {
                zox_tst_room_set(world, chunk, (byte3) { x, y, z }, block_filler_voxel(world, realm, door),
                    block_offset_encode((int3) { 0, -1, 0 }));
            } else {
                zox_tst_room_set(world, chunk, (byte3) { x, y, z }, bricks, 0);
            }
        }
        const byte glow = zox_getv(zox_tst_air_block(world, zox_id(BlockGlowStone)), BlockIndex);
        zox_tst_room_set(world, chunk, (byte3) { 2, 4, 6 }, glow, 0);
        const int3 c = zox_getv(chunk, ChunkPosition);
        const int length = octree_size(zox_getv(chunk, NodeDepth));
        zox_tst_dglow_base = (int3) { c.x * length + zox_tst_dglow_door_x, c.y * length + 4, c.z * length + 9 };
        zox_tst_dglow_stage = 1;
        return 0;
    }
    if (!zox_tst_room_settled(world, chunk, &zox_tst_dglow_settled)) {
        const char* waits[4] = { "", "the room built", "the sun in the open door", "the shut room's light" };
        zox_tst_waiting = waits[zox_tst_dglow_stage];
        return 0;
    }
    zox_tst_dglow_settled = 0;
    zox_tst_dglow_read(world, chunk, zox_tst_dglow_stage - 1);
    if (zox_tst_dglow_stage == 3) {
        zox_tst_dglow_stage = 4;
        return 1;
    }
    zox_tst_check(door_toggle(world, zox_tst_world_terrain, realm, zox_tst_dglow_base), "toggling the door did nothing");
    zox_tst_dglow_stage++;
    return 0;
}

void zox_tst_door_glow_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_dglow_stage == 4, "never got to the end, at stage [%i]", zox_tst_dglow_stage);
    const char* names[3] = { "built", "open", "shut" };
    for (byte s = 0; s < 3; s++) {
        zox_log("[tst] %s: by the glow [%i], middle [%i], by the door [%i]", names[s], zox_tst_dglow_lit[s][1][0][0],
            zox_tst_dglow_lit[s][2][0][1], zox_tst_dglow_lit[s][2][0][2]);
    }
    const byte built = zox_tst_dglow_lit[0][2][0][2], open = zox_tst_dglow_lit[1][2][0][2], shut = zox_tst_dglow_lit[2][2][0][2];
    zox_tst_check(zox_tst_dglow_lit[0][1][0][0] > darklight + 100, "built: the glow stone didn't light beside it [%i]",
        zox_tst_dglow_lit[0][1][0][0]);
    zox_tst_check(open > built + 20, "open: no light came in by the door [%i], shut it was [%i]", open, built);
    zox_tst_check(shut + 20 < open, "shut: by the door didn't darken [%i], open it was [%i]", shut, open);
    int lost = 0, kept = 0;
    for (byte x = 0; x < 6; x++) for (byte y = 0; y < 3; y++) for (byte z = 0; z < 3; z++) {
        if (x == 0 && y == 0 && z == 0) {
            continue;   // the glow stone
        }
        const byte before = zox_tst_dglow_lit[0][x][y][z], after = zox_tst_dglow_lit[2][x][y][z];
        lost += after + 4 < before;
        kept += after > before + 4;
    }
    zox_tst_check(!lost, "shut: [%i] voxels darker than the glow stone lit them", lost);
    zox_tst_check(!kept, "shut: [%i] voxels still brighter than the glow alone (the sun stayed)", kept);
}
