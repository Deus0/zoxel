// A town's roads and street lamps once its homes are placed (GenerateTown at town_generate_roads, fun/town.c): on
//  workers, within a budget a frame (a town's waypoint graph is well under a millisecond)
//  - a tunk draws a region's towns only once their roads are found (town_map_system waits)
#define zox_paths_budget_ms 1.0

void town_roads_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_budget(town_roads, zox_paths_budget_ms);
    zox_sys_in(BlockPosition2);
    zox_sys_in(TownSize);
    zox_sys_in(WallThickness);
    zox_sys_in(TownGateSide);
    zox_sys_in(TownGateAt);
    zox_sys_in(Seed);
    zox_sys_out(GenerateTown);
    zox_sys_out(RoadSegments);
    zox_sys_out(StreetLamps);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BlockPosition2, position);
        zox_sys_i(TownSize, size);
        zox_sys_i(WallThickness, thickness);
        zox_sys_i(TownGateSide, gate_side);
        zox_sys_i(TownGateAt, gate_at);
        zox_sys_i(Seed, seed);
        zox_sys_o(GenerateTown, generate);
        zox_sys_o(RoadSegments, roads);
        zox_sys_o(StreetLamps, lamps);
        if (generate->value != town_generate_roads || zox_sys_budget_spent(town_roads)) {
            continue;
        }
        zox_sys_budget_start(town_roads);
        TownHome homes[town_homes_max];
        byte homes_count = 0;
        entity children[zox_children_capacity];
        const uint count = zox_get_children_by_id(world, e, children, zox_children_capacity, zox_id(Home));
        for (uint k = 0; k < count && homes_count < town_homes_max; k++) {
            homes[homes_count++] = town_home(world, children[k]);
        }
        town_roads_build((uint) seed->value, position->value, size->value, thickness->value, gate_side->value,
            town_gate_at(position->value, size->value, gate_side->value, (int) gate_at->value - 128), homes, homes_count,
            roads, lamps);
        generate->value = 0;
        zox_sys_budget_end(town_roads);
    }
    zox_sys_on_end();
} zoxd_system(town_roads_system);

