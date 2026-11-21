// tst: town_build, the town building helpers (towns/fun/build.c, home.c, town.c), pure, no entities
//  - turning a building's frame and back, the way each facing faces
//  - a box's voxels in a chunk, at lods a thin box keeps a voxel
//  - a home's ground: its floor under it and a block round it, steps from its door, turned with it
//  - roads: the square and a road's legs, a road a few blocks wide
//  - street lamps beside roads, a door's and a bed's pieces turned with their home, highway crossings shared by
//    neighbouring regions
//  - run: bash bsh/tst.sh town_build
byte zox_tst_town_build_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_town_build_check(ecs* world) {
    (void) world;
    int failures = 0;

    // turning a building's frame and back
    for (int facing = 0; facing < 4; facing++) {
        for (int x = -3; x <= 3; x++) {
            for (int z = -3; z <= 3; z++) {
                int2 pos = { x, z };
                int2 turned = build_turn(facing, x, z);
                int2 unturned = build_unturn(facing, turned);
                if (!int2_equals(pos, unturned)) {
                    failures++;
                }
            }
        }
    }
    zox_tst_check(!failures, "[%i] turns don't turn back", failures);

    // the way each facing faces (its front, +z in its frame)
    zox_tst_check(build_turn(0, 0, 5).x == 0 && build_turn(0, 0, 5).y == 5, "build_turn facing 0");
    zox_tst_check(build_turn(1, 0, 5).x == 5 && build_turn(1, 0, 5).y == 0, "build_turn facing 1");
    zox_tst_check(build_turn(2, 0, 5).x == 0 && build_turn(2, 0, 5).y == -5, "build_turn facing 2");
    zox_tst_check(build_turn(3, 0, 5).x == -5 && build_turn(3, 0, 5).y == 0, "build_turn facing 3");

    // a box's voxels in a chunk, a thin box keeps a voxel at lods
    int from, to;
    zox_tst_check(build_span(10, 12, 0, 1, 16, &from, &to) == 1 && from == 10 && to == 12, "build_span 1");
    zox_tst_check(build_span(-5, -1, 0, 1, 16, &from, &to) == 0, "build_span 2");
    zox_tst_check(build_span(14, 20, 0, 1, 16, &from, &to) == 1 && from == 14 && to == 15, "build_span 3");
    zox_tst_check(build_span(5, 5, 0, 2, 8, &from, &to) == 1 && from == 2 && to == 2, "build_span 4");

    // a home's ground: its floor, its margin, steps from its door
    const TownHome home = { { 100, 100 }, { 3, 2 }, 0, 20, 0, 6 };
    zox_tst_check(home_ground_at(&home, (int2) { 100, 100 }, 25) == 20, "home_ground_at inside");
    zox_tst_check(home_ground_at(&home, (int2) { 104, 100 }, 25) == 20, "home_ground_at margin");
    zox_tst_check(home_ground_at(&home, (int2) { 100, 104 }, 30) == 21, "home_ground_at step +z");
    zox_tst_check(home_ground_at(&home, (int2) { 100, 104 }, 20) == 20, "home_ground_at level ground stays");
    zox_tst_check(home_ground_at(&home, (int2) { 110, 110 }, 25) == -1, "home_ground_at not its ground");

    // turned to face +x, its steps too
    const TownHome turned = { { 100, 100 }, { 3, 2 }, 0, 20, 1, 6 };
    zox_tst_check(home_ground_at(&turned, (int2) { 105, 100 }, 30) == 21, "home_ground_at turned +x");
    zox_tst_check(home_ground_at(&turned, (int2) { 100, 105 }, 30) == -1, "home_ground_at turned +z");

    // roads: the square, a road and its width
    TownRoads roads;
    roads.square = (int2) { 0, 0 };
    roads.count = 0;
    town_road(&roads, (int2) { 0, 20 }, (int2) { 0, 0 }, 1);
    zox_tst_check(town_is_road(&roads, (int2) { 0, 4 }) == 1, "town_is_road square");
    zox_tst_check(town_is_road(&roads, (int2) { 0, 10 }) == 1, "town_is_road road");
    zox_tst_check(town_is_road(&roads, (int2) { 1, 10 }) == 1, "town_is_road width");
    zox_tst_check(town_is_road(&roads, (int2) { 2, 10 }) == 0, "town_is_road outside width");
    zox_tst_check(town_is_road(&roads, (int2) { 10, 10 }) == 0, "town_is_road outside road");

    // street lamps: at the square's corners, beside a road on the spacing, not off its end
    zox_tst_check(town_is_lamp(&roads, (int2) { town_square_half + 1, town_square_half + 1 }), "town_is_lamp square corner");
    zox_tst_check(town_is_lamp(&roads, (int2) { town_road_half + 1, town_lamp_spacing }), "town_is_lamp beside road");
    zox_tst_check(!town_is_lamp(&roads, (int2) { town_road_half + 1, town_lamp_spacing + 1 }), "town_is_lamp off spacing");
    zox_tst_check(!town_is_lamp(&roads, (int2) { town_road_half + 1, town_lamp_spacing * 2 }), "town_is_lamp past road end");

    // a door and a bed in a home facing +x: the door's filler over its base, the bed's beside it, data back to the base
    for (byte facing = 0; facing < 4; facing++) {
        Building b = { .center = { 50, 50 }, .facing = facing, .floor = 10 };
        build_multi(&b, (int3) { 0, 2, 3 }, (int3) { 1, 2, 1 }, (int3) { 0, 0, 1 }, (int2) { 0, -1 }, 7, 9);
        build_multi(&b, (int3) { -2, 2, -2 }, (int3) { 2, 1, 1 }, (int3) { 1, 0, 0 }, (int2) { 1, 0 }, 8, 9);
        const int2 door = int2_add(b.center, build_turn(facing, 0, 3));
        const int2 bed_end = build_turn(facing, 1, 0);
        const byte pieces_ok = b.pieces_count == 4 &&
            int3_equals(b.pieces[0].at, ((int3) { door.x, 12, door.y })) && b.pieces[0].block == 7 &&
            int3_equals(b.pieces[1].at, ((int3) { door.x, 13, door.y })) && b.pieces[1].block == 9 &&
            int3_equals(int3_add(b.pieces[1].at, block_offset_decode(b.pieces[1].data)), b.pieces[0].at) &&
            int3_equals(int3_sub(b.pieces[3].at, b.pieces[2].at), ((int3) { bed_end.x, 0, bed_end.y })) &&
            int3_equals(int3_add(b.pieces[3].at, block_offset_decode(b.pieces[3].data)), b.pieces[2].at);
        zox_tst_check(pieces_ok, "a door's and bed's pieces wrong facing [%i]", facing);
        // the door's model +z points into the home (its front out)
        const int3 in = block_direction_offset(block_data_direction(b.pieces[0].data), (int3) { 0, 0, 1 });
        const int2 inward = build_turn(facing, 0, -1);
        zox_tst_check(in.x == inward.x && in.z == inward.y, "the door faces out wrong, facing [%i]", facing);
    }

    // highways: a border's crossing is the same from both regions sharing it
    HighwayEnd a[4], c[4];
    const int2 size = { 512, 512 };
    highway_crossings(1234, (int2) { 0, 0 }, (int2) { 0, 0 }, size, a);
    highway_crossings(1234, (int2) { 1, 0 }, (int2) { 512, 0 }, size, c);
    zox_tst_check(a[1].at.y == c[0].at.y && a[1].at.x + 1 == c[0].at.x, "east crossing [%ix%i] west of next [%ix%i]",
        a[1].at.x, a[1].at.y, c[0].at.x, c[0].at.y);
    highway_crossings(1234, (int2) { 0, 1 }, (int2) { 0, 512 }, size, c);
    zox_tst_check(a[3].at.x == c[2].at.x && a[3].at.y + 1 == c[2].at.y, "north crossing [%ix%i] south of next [%ix%i]",
        a[3].at.x, a[3].at.y, c[2].at.x, c[2].at.y);
    zox_tst_check(a[0].at.y >= 128 && a[0].at.y < 384, "crossing [%i] not in its border's middle half", a[0].at.y);

    zox_log("[tst] [metric] town_build checked turns, spans, home ground, roads, lamps, furniture, crossings");
}

void zox_tsts_town_build(ecs* world) {
    zox_tst_add("town_build", zox_tst_town_build_ready, zox_tst_town_build_check, 30);
}
