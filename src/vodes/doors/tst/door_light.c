// tst: door_light, light goes through a door only while it's open (block_light_pass_open, lights3 light_stops)
//  - the player's game; a sealed tunnel two tall under the ground (terrain tst glow_relight's room), a glow stone at
//    one end, a door across it (its base and filler), the far side past it
//  - shut: the far side dark; opened (door_toggle): lit through it; shut again: dark again, the glow side still lit
//  - the door's block vox brightness (get_block_vox_brightness): shut, its darker side's (the far one's), not the glow
//    side's (lit by its brightest side it glowed in a dark room); open, lit by the glow
//  - run: bash bsh/tst.sh door_light
#define zox_tst_door_light_x 3      // the door's x along the tunnel, the glow stone at 5, the far side at 1
byte zox_tst_door_light_stage = 0;
int zox_tst_door_light_settled = 0;
entity zox_tst_door_light_chunk = 0;
byte3 zox_tst_door_light_at;
byte zox_tst_door_light_far[3];     // shut, open, shut again: the far side's light (its lower voxel)
byte zox_tst_door_light_near[3];    // beside the glow stone
float zox_tst_door_light_vox[3];    // the door's block vox brightness, its base and filler's brightest

static inline void zox_tst_door_light_read(ecs* world, byte stage) {
    const byte3 a = zox_tst_door_light_at;
    zox_tst_door_light_far[stage] = zox_tst_room_light(world, zox_tst_door_light_chunk, (byte3) { a.x + 1, a.y - 1, a.z });
    zox_tst_door_light_near[stage] = zox_tst_room_light(world, zox_tst_door_light_chunk, (byte3) { a.x + 4, a.y, a.z });
    const float base = get_block_vox_brightness(world, zox_tst_door_light_chunk, (byte3) { a.x + zox_tst_door_light_x, a.y - 1, a.z });
    const float top = get_block_vox_brightness(world, zox_tst_door_light_chunk, (byte3) { a.x + zox_tst_door_light_x, a.y, a.z });
    zox_tst_door_light_vox[stage] = base > top ? base : top;
}

static inline void zox_tst_door_light_toggle(ecs* world) {
    const entity character = zox_tst_player_character(world);
    const byte3 a = zox_tst_door_light_at;
    const int3 base = chunk_block_position(world, zox_tst_door_light_chunk, (byte3) { a.x + zox_tst_door_light_x, a.y - 1, a.z });
    zox_tst_check(door_toggle(world, zox_get_link(world, character, TerrainLink), zox_get_link(world, character, RealmLink), base),
        "toggling the door did nothing");
}

byte zox_tst_door_light_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_progress(zox_tst_door_light_stage * 100 + zox_tst_door_light_settled);
    if (zox_tst_door_light_stage == 0) {
        zox_tst_waiting = "a sealed room under the ground near the player";
        zox_tst_door_light_chunk = zox_tst_room_chunk(world, 6, 2, 0, &zox_tst_door_light_at);
        if (!zox_tst_door_light_chunk) {
            return 0;
        }
        const entity chunk = zox_tst_door_light_chunk;
        const entity realm = zox_get_link(world, zox_tst_player_character(world), RealmLink);
        const byte3 a = zox_tst_door_light_at;
        zox_log("[tst] room at [%ix%ix%i]", a.x, a.y, a.z);
        for (byte i = 0; i < 5; i++) for (byte y = 0; y < 2; y++) {
            if (i == zox_tst_door_light_x) {
                continue;   // the door's, one edit a voxel
            }
            zox_tst_room_set(world, chunk, (byte3) { a.x + i, a.y - y, a.z }, 0, 0);
        }
        const entity glow = zox_get_child_by_id(world, realm, zox_id(BlockGlowStone));
        const entity door = zox_get_child_by_id(world, realm, zox_id(BlockDoor));
        zox_tst_room_set(world, chunk, (byte3) { a.x + 5, a.y, a.z }, zox_getv(glow, BlockIndex), 0);
        // its base facing 0 (block data 1 + direction), its filler a block over pointing down to it
        zox_tst_room_set(world, chunk, (byte3) { a.x + zox_tst_door_light_x, a.y - 1, a.z }, zox_getv(door, BlockIndex), 1);
        zox_tst_room_set(world, chunk, (byte3) { a.x + zox_tst_door_light_x, a.y, a.z }, block_filler_voxel(world, realm, door),
            block_offset_encode((int3) { 0, -1, 0 }));
        zox_tst_door_light_stage = 1;
        return 0;
    }
    if (!zox_tst_room_settled(world, zox_tst_door_light_chunk, &zox_tst_door_light_settled)) {
        zox_tst_waiting = zox_tst_door_light_stage == 2 ? "light through the open door" : "the door shut, the room's light";
        return 0;
    }
    zox_tst_door_light_read(world, zox_tst_door_light_stage - 1);
    if (zox_tst_door_light_stage == 3) {
        return 1;
    }
    zox_tst_door_light_toggle(world);
    zox_tst_door_light_settled = 0;
    zox_tst_door_light_stage++;
    return 0;
}

void zox_tst_door_light_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_door_light_stage == 3, "never got to the end, at stage [%i]", zox_tst_door_light_stage);
    zox_log("[tst] far side: shut [%i] open [%i] shut [%i], beside the glow stone [%i] [%i] [%i]",
        zox_tst_door_light_far[0], zox_tst_door_light_far[1], zox_tst_door_light_far[2],
        zox_tst_door_light_near[0], zox_tst_door_light_near[1], zox_tst_door_light_near[2]);
    zox_log("[tst] the door's brightness: shut [%.2f] open [%.2f] shut [%.2f]",
        zox_tst_door_light_vox[0], zox_tst_door_light_vox[1], zox_tst_door_light_vox[2]);
    for (byte s = 0; s < 3; s += 2) {
        zox_tst_check(zox_tst_door_light_vox[s] <= block_vox_light_to_brightness(zox_tst_door_light_far[s] + 2),
            "the shut door is lit [%.2f], brighter than its dark side [%.2f]", zox_tst_door_light_vox[s],
            block_vox_light_to_brightness(zox_tst_door_light_far[s]));
    }
    zox_tst_check(zox_tst_door_light_vox[1] > block_vox_light_to_brightness(darklight + 40), "the open door is dark [%.2f]",
        zox_tst_door_light_vox[1]);
    for (byte s = 0; s < 3; s++) {
        zox_tst_check(zox_tst_door_light_near[s] > darklight + 150, "beside the glow stone is [%i]", zox_tst_door_light_near[s]);
    }
    zox_tst_check(zox_tst_door_light_far[0] <= darklight, "light went through the shut door [%i]", zox_tst_door_light_far[0]);
    zox_tst_check(zox_tst_door_light_far[1] > darklight + 40, "no light through the open door [%i]", zox_tst_door_light_far[1]);
    zox_tst_check(zox_tst_door_light_far[2] <= darklight, "light still through the door shut again [%i]", zox_tst_door_light_far[2]);
}
