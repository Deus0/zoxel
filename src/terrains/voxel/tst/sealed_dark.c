// tst: sealed_dark and sealed_dark_top, a room walled in where the sun was goes dark (doc/tech/lighting.md, Dark): no
//  emitter inside, so every voxel in it ends at darklight
//  - an all air chunk in the void (air_chunk.c), fully sunlit, a brick box 7 a side built in one edit, as a player
//    walling and roofing a room in one go
//    - sealed_dark: in the chunk's middle (y 3 to 9)
//    - sealed_dark_top: its roof the chunk's top row (y 9 to 15), the sun's beams come in from the chunk above there
//  - checks: its inside's brightest voxel is darklight; logs its middle column, floor up, and where it's lit if it is
//  - run: bash bsh/tst.sh sealed_dark
byte zox_tst_sdark_stage = 0;
int zox_tst_sdark_settled = 0;
byte zox_tst_sdark_floor = 3;   // the box's floor's y, its roof 6 up

byte zox_tst_sdark_ready(ecs* world, byte floor) {
    const entity chunk = zox_tst_streamed_air_chunk(world);
    if (!chunk) {
        return 0;
    }
    if (zox_tst_sdark_stage == 0) {
        zox_tst_sdark_floor = floor;
        const byte bricks = zox_getv(zox_tst_air_block(world, zox_id(BlockBricks)), BlockIndex);
        for (byte x = 4; x <= 10; x++) for (byte y = floor; y <= floor + 6; y++) for (byte z = 4; z <= 10; z++) {
            if (!(x >= 5 && x <= 9 && y > floor && y < floor + 6 && z >= 5 && z <= 9)) {
                zox_tst_room_set(world, chunk, (byte3) { x, y, z }, bricks, 0);
            }
        }
        zox_tst_sdark_stage = 1;
        return 0;
    }
    zox_tst_waiting = "the box built and its lights settled";
    return zox_tst_room_settled(world, chunk, &zox_tst_sdark_settled);
}

byte zox_tst_sealed_dark_ready(ecs* world) {
    return zox_tst_sdark_ready(world, 3);
}

byte zox_tst_sealed_dark_top_ready(ecs* world) {
    return zox_tst_sdark_ready(world, 9);
}

void zox_tst_sealed_dark_check(ecs* world) {
    const entity chunk = zox_tst_air_found;
    const byte floor = zox_tst_sdark_floor;
    char column[96];
    int n = 0;
    for (byte y = floor; y <= floor + 6; y++) {
        n += snprintf(column + n, sizeof(column) - n, " %i", zox_tst_room_light(world, chunk, (byte3) { 7, y, 7 }));
    }
    int brightest = 0, lit = 0;
    for (byte x = 5; x <= 9; x++) for (byte y = floor + 1; y <= floor + 5; y++) for (byte z = 5; z <= 9; z++) {
        const int light = zox_tst_room_light(world, chunk, (byte3) { x, y, z });
        brightest = light > brightest ? light : brightest;
        lit += light > darklight;
    }
    zox_log("[tst] sealed_dark: its middle column, floor (y %i) to roof [%s ], brightest inside [%i], [%i] of 125 lit",
        floor, column, brightest, lit);
    // where it's lit, a row a z, when it fails
    for (byte y = floor + 1; y <= floor + 5 && brightest > darklight; y++) {
        char row[256];
        int m = 0;
        for (byte z = 5; z <= 9; z++) {
            for (byte x = 5; x <= 9; x++) {
                m += snprintf(row + m, sizeof(row) - m, "%4i", zox_tst_room_light(world, chunk, (byte3) { x, y, z }));
            }
            m += snprintf(row + m, sizeof(row) - m, " |");
        }
        zox_log("[tst] sealed_dark: y [%i] (rows z 5 to 9, x 5 to 9) %s", y, row);
    }
    zox_tst_check(brightest <= darklight, "the sealed room's inside is lit: brightest [%i], [%i] of 125 voxels over darklight [%i]",
        brightest, lit, darklight);
}
