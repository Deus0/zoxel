// tst: town_maps, towns draw into every tunk they cross, walls join up across tunk edges
//  - each generated tunk at full detail: for every town in its region, every column of the tunk inside it
//    (not region_tunk_box, which the maps use to draw)
//  - its outer ring is wall (a wall, edge, spike or the gate), every column inside the town is marked
//  - counts towns crossing tunks, so walls drawn by more than one tunk are checked
//  - run: bash bsh/tst.sh town_maps
int zox_tst_town_maps_frames = 0;

byte zox_tst_town_maps_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    // a few frames for nearby tunks' towns to draw
    zox_tst_waiting = "towns to draw into tunks";
    return ++zox_tst_town_maps_frames > 30;
}

static inline byte zox_tst_is_town_wall(byte value) {
    return value == zox_town_type_wall || value == zox_town_type_wall_edge ||
        value == zox_town_type_wall_spike || value == zox_town_type_gate;
}

void zox_tst_town_maps_check(ecs* world) {
    int tunks = 0, columns = 0, ring_wrong = 0, inside_unmarked = 0;
    // towns seen, and how many tunks each was drawn in
    entity towns_seen[256];
    int towns_tunks[256];
    int towns_count = 0;
    iter it2 = zox_children(world, local_terrain);
    while (zox_children_next(it2)) {
        for (int i = 0; i < it2.count; i++) {
            const entity tunk = it2.entities[i];
            // drawn tunks (GenerateTunk is back to 0 once a tunk is done, its town map is sized)
            if (!zox_has(tunk, TownMap) || !zox_has(tunk, TunkPosition) || !zox_has(tunk, TunkLod) ||
                zox_getv(tunk, GenerateTunk))
            {
                continue;
            }
            RegionTunk rt;
            if (!region_tunk_begin(world, tunk, zox_getv(tunk, TunkLod), zox_getv(tunk, TunkPosition), &rt) ||
                rt.depth_difference != 1)
            {
                continue;
            }
            const TownMap* town_map = zox_get(tunk, TownMap);
            if (town_map->length != rt.length * rt.length) {
                continue;
            }
            tunks++;
            entity towns[zox_children_capacity];
            const uint towns_length = zox_get_children_by_id(world, rt.region, towns, zox_children_capacity, zox_id(Town));
            for (uint j = 0; j < towns_length; j++) {
                const int2 center = zox_getv(towns[j], BlockPosition2);
                const byte2 size = zox_getv(towns[j], TownSize);
                const int left = center.x - size.x / 2, right = center.x + size.x / 2;
                const int bottom = center.y - size.y / 2, top = center.y + size.y / 2;
                // every column of the tunk, not region_tunk_box (the maps use it, a bug there would hide here)
                int2 position;
                byte in_tunk = 0;
                for (position.x = 0; position.x < rt.length && !in_tunk; position.x++) {
                    for (position.y = 0; position.y < rt.length && !in_tunk; position.y++) {
                        const int2 g = region_tunk_global(&rt, position);
                        in_tunk = g.x >= left && g.x <= right && g.y >= bottom && g.y <= top;
                    }
                }
                if (!in_tunk) {
                    continue;
                }
                int t = 0;
                for (; t < towns_count && towns_seen[t] != towns[j]; t++) { }
                if (t == towns_count && towns_count < 256) {
                    towns_seen[towns_count] = towns[j];
                    towns_tunks[towns_count++] = 0;
                }
                if (t < 256) {
                    towns_tunks[t]++;
                }
                for (position.x = 0; position.x < rt.length; position.x++) {
                    for (position.y = 0; position.y < rt.length; position.y++) {
                        const int2 g = region_tunk_global(&rt, position);
                        if (g.x < left || g.x > right || g.y < bottom || g.y > top) {
                            continue;
                        }
                        const byte value = town_map->value[int2_array_index(position, rt.map_size)];
                        columns++;
                        if (g.x == left || g.x == right || g.y == bottom || g.y == top) {
                            ring_wrong += !zox_tst_is_town_wall(value);
                        } else {
                            inside_unmarked += value == zox_town_type_none;
                        }
                    }
                }
            }
        }
    }
    int crossing = 0;
    for (int t = 0; t < towns_count; t++) {
        crossing += towns_tunks[t] > 1;
    }
    zox_log("[tst] [metric] town_maps tunks [%i] towns [%i] crossing tunks [%i] columns checked [%i]",
        tunks, towns_count, crossing, columns);
    zox_tst_check(towns_count > 0, "no towns drawn near the player in [%i] tunks", tunks);
    zox_tst_check(crossing > 0, "no town crosses tunks, the across tunks case isn't checked");
    zox_tst_check(!ring_wrong, "[%i] columns on a town's outer ring aren't wall", ring_wrong);
    zox_tst_check(!inside_unmarked, "[%i] columns inside a town aren't marked", inside_unmarked);
}
