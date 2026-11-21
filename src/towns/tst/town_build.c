// tst: town_build, the town building helpers (towns/fun/build.c, home.c, town.c), pure, no entities
//  - turning a building's frame and back, the way each facing faces
//  - a box's voxels in a chunk, at lods a thin box keeps a voxel
//  - a home's ground: its floor under it and a block round it, steps from its door, turned with it
//  - A*: straight over open ground, through a wall's gap, round a hill
//  - a town's wall: grown districts in its rect, not a box, its gate on it, the same from its seed
//  - town roads round a home in their way, to each door and the gate; a road's width and its ground's slope
//  - a door's and a bed's pieces turned with their home, highway nodes in their cells
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

    // A* (fun/astar.c): on flat open ground a straight line, through a wall's gap, round a hill
    static PathGrid grid;
    int3 points[path_points_max];
    path_grid_begin(&grid, (int2) { 0, 0 }, (int2) { 32, 32 }, 1);
    for (int i = 0; i < grid.size.x * grid.size.y; i++) {
        grid.heights[i] = 10;
        grid.costs[i] = path_cost_ground;
    }
    int count = path_find(&grid, (int2) { 2, 2 }, (int2) { 28, 20 }, points, path_points_max);
    zox_tst_check(count == 2, "a path over flat open ground is [%i] points, not one straight segment", count);
    // a wall across x 16, a gap at z 20 to 22
    path_grid_rect(&grid, (int2) { 16, 9 }, (int2) { 0, 9 }, path_blocked);
    path_grid_rect(&grid, (int2) { 16, 27 }, (int2) { 0, 4 }, path_blocked);
    count = path_find(&grid, (int2) { 4, 4 }, (int2) { 28, 4 }, points, path_points_max);
    int crossings_wrong = 0, through_gap = 0;
    for (int k = 0; k + 1 < count; k++) {
        const int steps = int_max(abs(points[k + 1].x - points[k].x), abs(points[k + 1].z - points[k].z)) * 2;
        for (int m = 0; m <= steps; m++) {
            const float f = steps ? (float) m / steps : 0;
            const int2 g = { (int) roundf(points[k].x + (points[k + 1].x - points[k].x) * f), (int) roundf(points[k].z + (points[k + 1].z - points[k].z) * f) };
            crossings_wrong += grid.costs[path_index(&grid, g)] == path_blocked;
            through_gap += g.x == 16 && g.y >= 19 && g.y <= 23;
        }
    }
    zox_tst_check(count >= 3 && !crossings_wrong && through_gap, "a path through a wall's gap: [%i] points, [%i] blocked crossed, [%i] in the gap",
        count, crossings_wrong, through_gap);
    // a hill across its way (20 high, x 12 to 20, z 0 to 24): it goes round
    for (int i = 0; i < grid.size.x * grid.size.y; i++) {
        const int2 c = { i % grid.size.x, i / grid.size.x };
        grid.costs[i] = path_cost_ground;
        grid.heights[i] = c.x >= 12 && c.x <= 20 && c.y <= 24 ? 30 : 10;
    }
    count = path_find(&grid, (int2) { 2, 4 }, (int2) { 30, 4 }, points, path_points_max);
    int up_hill = 0;
    for (int k = 0; k < count; k++) {
        up_hill += points[k].y > 10;
    }
    zox_tst_check(count >= 3 && !up_hill, "a path over a hill: [%i] points, [%i] up it, not round it", count, up_hill);
    // a segment crossing a rect, missing it, and along its edge
    zox_tst_check(segment_crosses_rect((float2) { -10, 0 }, (float2) { 10, 0 }, (int2) { 0, 0 }, (float2) { 2, 2 }), "segment_crosses_rect across");
    zox_tst_check(!segment_crosses_rect((float2) { -10, 5 }, (float2) { 10, 5 }, (int2) { 0, 0 }, (float2) { 2, 2 }), "segment_crosses_rect past");
    zox_tst_check(!segment_crosses_rect((float2) { -10, -10 }, (float2) { -3, -3 }, (int2) { 0, 0 }, (float2) { 2, 2 }), "segment_crosses_rect short");
    // a town's wall (fun/town.c): grown districts inside its rect, its corners cut, its gate on it, the same from its seed
    {
        int outside_rect = 0, boxes = 0;
        for (lint seed = 1; seed < 40; seed++) {
            const TownShape shape = town_shape(seed * 977, (int2) { 0, 0 }, (byte2) { 100, 70 }, 8, seed % 4, 0);
            int inside = 0;
            for (int x = -60; x <= 60; x++) for (int z = -45; z <= 45; z++) {
                const byte in = town_shape_depth(&shape, (int2) { x, z }) >= 0;
                inside += in;
                outside_rect += in && (abs(x) > 50 || abs(z) > 35);
            }
            boxes += inside > 100 * 70 * 9 / 10;
        }
        zox_tst_check(!outside_rect, "[%i] of towns' walls' columns outside their rects", outside_rect);
        zox_tst_check(boxes < 4, "[%i] of 39 towns fill their rects (boxes)", boxes);
        const TownShape shape = town_shape(1234, (int2) { 0, 0 }, (byte2) { 100, 70 }, 8, 1, 3);
        zox_tst_check(town_shape_depth(&shape, (int2) { 49, 34 }) < 0, "a town wall's corner isn't cut (a box)");
        zox_tst_check(town_wall_value(&shape, shape.gate) == zox_town_type_gate, "its gate [%i %i] isn't in its wall [%i]",
            shape.gate.x, shape.gate.y, town_wall_value(&shape, shape.gate));
        zox_tst_check(town_shape_depth(&shape, int2_add(shape.gate, build_turn(1, 0, 1))) < 0, "its gate isn't on its outer edge");
        zox_tst_check(town_wall_value(&shape, (int2) { 0, 0 }) == zox_town_type_road, "its middle isn't its square");
        const TownShape again = town_shape(1234, (int2) { 0, 0 }, (byte2) { 100, 70 }, 8, 1, 3);
        zox_tst_check(again.power == shape.power && int2_equals(again.gate, shape.gate), "a town wall isn't the same from its seed");
    }
    // a town's roads (fun/town.c): a home stands between another's door and the square, its road goes round it
    {
        const TownHome homes[2] = {
            { { 0, -22 }, { 3, 3 }, 0, 10, 0, 6 },     // its door +z at 0 -18, the square at 0 0
            { { 0, -11 }, { 4, 2 }, 0, 10, 1, 6 },     // across the way, its door +x
        };
        RoadSegments roads = { 0 };
        StreetLamps lamps = { 0 };
        const TownShape shape = town_shape(1234, (int2) { 0, 0 }, (byte2) { 80, 80 }, 6, 0, 0);
        town_roads_build(1234, &shape, homes, 2, &roads, &lamps);
        int crossing = 0, door_roads = 0, gate_road = 0;
        for (int i = 0; i + 1 < roads.length; i += 2) {
            const int3 a = roads.value[i], b = roads.value[i + 1];
            for (byte h = 0; h < 2; h++) {
                crossing += segment_crosses_rect((float2) { a.x, a.z }, (float2) { b.x, b.z }, homes[h].center,
                    (float2) { homes[h].half.x - 0.5f, homes[h].half.y - 0.5f });
            }
            door_roads += (a.x == 0 && a.z == -18) || (a.x == 5 && a.z == -11);
            gate_road += a.x == shape.gate.x && a.z == shape.gate.y;
        }
        zox_tst_check(!crossing, "[%i] town road segments cross homes", crossing);
        zox_tst_check(door_roads == 2, "[%i] of 2 doors have a road", door_roads);
        zox_tst_check(gate_road, "no road through the gate");
        zox_tst_check(lamps.length > 0, "no street lamps");
        dispose_RoadSegments(&roads);
        dispose_StreetLamps(&lamps);
    }
    // highways (fun/highways.c): a node in its cell's middle half, each node links on
    int nodes_wrong = 0, links_none = 0;
    for (int x = -4; x <= 4; x++) for (int y = -4; y <= 4; y++) {
        const int2 n = highway_node_at(1234, (int2) { x, y });
        const int2 in = { n.x - x * highway_spacing, n.y - y * highway_spacing };
        nodes_wrong += in.x < highway_spacing / 4 || in.x >= highway_spacing * 3 / 4 || in.y < highway_spacing / 4 || in.y >= highway_spacing * 3 / 4;
        links_none += !highway_links(1234, (int2) { x, y });
    }
    zox_tst_check(!nodes_wrong, "[%i] highway nodes out of their cell's middle", nodes_wrong);
    zox_tst_check(!links_none, "[%i] highway nodes link nowhere", links_none);
    // a road's width and its ground along a segment
    RoadSegments segments = { 0 };
    const int3 segment[2] = { { 0, 10, 0 }, { 10, 20, 0 } };
    road_segments_add(&segments, segment, 2);
    int height = 0;
    zox_tst_check(road_segments_at(&segments, (int2) { 5, 1 }, 1.5f, &height) && height == 15, "road_segments_at middle [%i]", height);
    zox_tst_check(!road_segments_at(&segments, (int2) { 5, 3 }, 1.5f, NULL), "road_segments_at past its width");
    dispose_RoadSegments(&segments);

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

    zox_log("[tst] [metric] town_build checked turns, spans, home ground, A*, roads, furniture, highway nodes");
}

void zox_tsts_town_build(ecs* world) {
    zox_tst_add("town_build", zox_tst_town_build_ready, zox_tst_town_build_check, 30);
}
