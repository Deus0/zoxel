// tst: door_sky, a room lit through its open door keeps that light when a hole in its ceiling is dug and covered
//  (lights3/sys/dark.c dark_flood_light)
//  - the player's game; a sealed tunnel two tall under sunlit ground: a shaft to the sky over its start (outside),
//    an open door at x 2, the room past it (x 3 to 6), sunlit through the door
//  - a hole dug from the sky over the room's end (x 6): the sun comes in; covered: its dark floods the room, the
//    light through the door stays (each voxel darkened only if it was lit from the one before it, its light less)
//  - was: the dark took every voxel dimmer than the hole's sun, the door's light too
//  - run: bash bsh/tst.sh door_sky
#define zox_tst_dsky_width 7
#define zox_tst_dsky_hole 3      // the hole over the room, beside the door
byte zox_tst_dsky_stage = 0;
int zox_tst_dsky_settled = 0;
entity zox_tst_dsky_chunk = 0;
byte3 zox_tst_dsky_at;
byte zox_tst_dsky_top = 0;
byte zox_tst_dsky_lit[3][4];    // the room (x 3 to 6, lower row): through the door, sun in the hole, covered
byte zox_tst_dsky_outside = 0;

static inline void zox_tst_dsky_read(ecs* world, byte stage) {
    const byte3 a = zox_tst_dsky_at;
    for (byte i = 0; i < 4; i++) {
        zox_tst_dsky_lit[stage][i] = zox_tst_room_light(world, zox_tst_dsky_chunk, (byte3) { a.x + 3 + i, a.y - 1, a.z });
    }
}

byte zox_tst_door_sky_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_progress(zox_tst_dsky_stage * 100 + zox_tst_dsky_settled);
    const entity chunk = zox_tst_dsky_chunk;
    const byte3 a = zox_tst_dsky_at;
    const entity character = zox_tst_player_character(world);
    const entity realm = zox_get_link(world, character, RealmLink);
    if (zox_tst_dsky_stage == 0) {
        zox_tst_waiting = "a sealed room under sunlit ground near the player";
        zox_tst_dsky_chunk = zox_tst_room_chunk(world, zox_tst_dsky_width, 2, 1, &zox_tst_dsky_at);
        if (!zox_tst_dsky_chunk) {
            return 0;
        }
        const entity c = zox_tst_dsky_chunk;
        const byte3 at = zox_tst_dsky_at;
        zox_log("[tst] room at [%ix%ix%i]", at.x, at.y, at.z);
        for (byte i = 0; i < zox_tst_dsky_width; i++) for (byte y = 0; y < 2; y++) {
            if (i != 2) {
                zox_tst_room_set(world, c, (byte3) { at.x + i, at.y - y, at.z }, 0, 0);
            }
        }
        // the shaft to the sky over its start
        zox_tst_room_set(world, c, (byte3) { at.x, at.y + 1, at.z }, 0, 0);
        zox_tst_room_set(world, c, (byte3) { at.x, at.y + 2, at.z }, 0, 0);
        // the door, open: its base's block data 1 + direction, open; its filler over it
        const entity door = zox_get_child_by_id(world, realm, zox_id(BlockDoor));
        zox_tst_room_set(world, c, (byte3) { at.x + 2, at.y - 1, at.z }, zox_getv(door, BlockIndex), 1 | block_data_open);
        zox_tst_room_set(world, c, (byte3) { at.x + 2, at.y, at.z }, block_filler_voxel(world, realm, door),
            block_offset_encode((int3) { 0, -1, 0 }) | block_data_open);
        zox_tst_dsky_stage = 1;
        return 0;
    }
    if (!zox_tst_room_settled(world, chunk, &zox_tst_dsky_settled)) {
        zox_tst_waiting = "the room's light to settle";
        return 0;
    }
    zox_tst_dsky_settled = 0;
    if (zox_tst_dsky_stage == 1) {
        zox_tst_dsky_outside = zox_tst_room_light(world, chunk, (byte3) { a.x, a.y - 1, a.z });
        zox_tst_dsky_read(world, 0);
        // the hole over the room's end, from the sky (its surface, if sunlit there, else it's only the ceiling)
        zox_tst_dsky_top = getv_VoxelNode(zox_get(chunk, VoxelNode), zox_getv(chunk, NodeDepth), (byte3) { a.x + zox_tst_dsky_hole, a.y + 2, a.z });
        zox_tst_room_set(world, chunk, (byte3) { a.x + zox_tst_dsky_hole, a.y + 2, a.z }, 0, 0);
        zox_tst_room_set(world, chunk, (byte3) { a.x + zox_tst_dsky_hole, a.y + 1, a.z }, 0, 0);
        zox_tst_dsky_stage = 2;
        return 0;
    }
    if (zox_tst_dsky_stage == 2) {
        zox_tst_dsky_read(world, 1);
        zox_tst_room_set(world, chunk, (byte3) { a.x + zox_tst_dsky_hole, a.y + 2, a.z }, zox_tst_dsky_top, 0);
        zox_tst_dsky_stage = 3;
        return 0;
    }
    zox_tst_dsky_read(world, 2);
    zox_tst_dsky_stage = 4;
    return 1;
}

void zox_tst_door_sky_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_dsky_stage == 4, "never got to the end, at stage [%i]", zox_tst_dsky_stage);
    const char* names[3] = { "through the door", "sun in the hole", "covered" };
    for (byte s = 0; s < 3; s++) {
        const byte* l = zox_tst_dsky_lit[s];
        zox_log("[tst] %s [%i %i %i %i], outside [%i]", names[s], l[0], l[1], l[2], l[3], zox_tst_dsky_outside);
    }
    zox_tst_check(zox_tst_dsky_outside == sunlight, "the shaft outside isn't sunlit [%i]", zox_tst_dsky_outside);
    zox_tst_check(zox_tst_dsky_lit[0][0] > darklight + 40, "no light through the open door [%i]", zox_tst_dsky_lit[0][0]);
    int lost = 0;
    for (byte i = 0; i < 4; i++) {
        lost += zox_tst_dsky_lit[2][i] + 4 < zox_tst_dsky_lit[0][i];
    }
    zox_tst_check(!lost, "covered: [%i] room voxels darker than the open door lit them", lost);
}
