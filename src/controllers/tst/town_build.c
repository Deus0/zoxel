// tst: town_build, the town building helpers (towns/fun/build.c, home.c, town.c), pure, no entities
//  - turning a building's frame and back, the way each facing faces
//  - a box's voxels in a chunk, at lods a thin box keeps a voxel
//  - a home's ground: its floor under it and a block round it, steps from its door, turned with it
//  - roads: the square and a road's legs, a road a few blocks wide
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

    zox_log("[tst] [metric] town_build checked turns, spans, home ground, roads");
}
