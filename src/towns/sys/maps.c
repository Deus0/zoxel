// a road's segments (pairs of points, fun/paths.c) into a tunk's maps: columns within half of one (more at lods, a
//  column a cell) where the map holds over (a town's zone, or nothing outside towns) are value, their ground set
//  along the segment's slope if it has one (cut or filled, the road level with its way), but in homes' margins and
//  steps; no trees on them
static inline void town_map_roads(const RegionTunk* rt, const int3* segments, int segments_count, float half, byte over,
    byte value, const TownHome* homes, byte homes_count, TownMap* town_map, HeightMap* height_map, VegetationMap* vegetation_map)
{
    const int dd = rt->depth_difference;
    for (int i = 0; i + 1 < segments_count; i += 2) {
        const int3 a = segments[i], b = segments[i + 1];
        int2 position, from, to;
        if (!road_segment_columns(rt, a, b, (int) ceilf(half), &from, &to)) {
            continue;
        }
        for (position.x = from.x; position.x <= to.x; position.x++) {
            for (position.y = from.y; position.y <= to.y; position.y++) {
                const int2 g = region_tunk_global(rt, position);
                float t;
                const float2 middle = { g.x + (dd - 1) * 0.5f, g.y + (dd - 1) * 0.5f };
                if (road_segment_distance(a, b, middle, &t) > half + (dd - 1) * 0.5f) {
                    continue;
                }
                const int index = int2_array_index(position, rt->map_size);
                const byte v = town_map->value[index];
                if (v != over && v != value) {
                    continue;
                }
                town_map->value[index] = value;
                byte stepped = 0;
                for (byte h = 0; h < homes_count && !stepped; h++) {
                    stepped = home_ground_at(&homes[h], g, 0) >= 0;
                }
                if (!stepped && a.y >= 0 && b.y >= 0) {
                    height_map->value[index] = (byte) int_clamp((int) roundf(a.y + (b.y - a.y) * t), 0, 255);
                }
                if (!zox_disable_vegetation && vegetation_map->length) {
                    vegetation_map->value[index] = 0;
                }
            }
        }
    }
}

#define highway_ends_max_tunk 64

// NOTE: This builds out Simple Town Data into 2D Maps!
void town_map_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    // byte is_effect_heights = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TunkLod);
    zox_sys_in(TunkPosition);
    zox_sys_in(BiomeMap);
    zox_sys_out(GenerateTunk);
    zox_sys_out(HeightMap);
    zox_sys_out(VegetationMap);
    zox_sys_out(TownMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TunkLod, lod);
        zox_sys_i(TunkPosition, tunk_position);
        zox_sys_i(BiomeMap, biome_map);
        zox_sys_o(GenerateTunk, generate);
        zox_sys_o(HeightMap, height_map);
        zox_sys_o(VegetationMap, vegetation_map);
        zox_sys_o(TownMap, town_map);
        if (generate->value != zox_generate_tunk_towns) {
            continue;
        }
        // not disabled for 2, thats maps only
        if (zox_disable_towns == 1) {
            generate->value = zox_generate_tunk_end;
            continue;
        }
        // its region and sizes (regions/fun/tunk.c)
        RegionTunk rt;
        if (!region_tunk_begin(world, e, lod->value, tunk_position->value, &rt)) {
            continue;
        }
        const entity region = rt.region;
        const byte terrain_length = rt.terrain_length;
        const byte length = rt.length;
        const int2 map_size = rt.map_size;
        const int2 global_position_start = rt.global_start;
#ifdef zox_safety_checks
        if (!zox_disable_biomes && !biome_map->length) {
            zox_loge("[%s]'s TownMap: [BiomeMap] Invalid", zox_get_name(e));
            continue;
        }
        if (!height_map->length) {
            zox_loge("[%s]'s TownMap: [HeightMap] Invalid", zox_get_name(e));
            continue;
        }
        if (!zox_disable_vegetation && !vegetation_map->length) {
            zox_loge("[%s]'s TownMap: [VegetationMap] Invalid", zox_get_name(e));
            continue;
        }
#endif
        // its towns' roads first (sys/roads.c)
        entity waiting[zox_children_capacity];
        const uint waiting_count = zox_get_children_by_id(world, region, waiting, zox_children_capacity, zox_id(Town));
        byte roads_pending = 0;
        for (uint w = 0; w < waiting_count && !roads_pending; w++) {
            roads_pending = zox_getv(waiting[w], GenerateTown) != 0;
        }
        if (roads_pending) {
            continue;
        }
        // the highways crossing it, worked out from the nodes round it (fun/highways.c); later if a region round it is
        //  still placing its towns
        Highways highways;
        highways_begin(&highways, world, zox_get_parent(world, e), global_position_start);
        int2 highway_ends[highway_ends_max_tunk];
        const int highways_count = highways_in_box(&highways, global_position_start,
            int2_add(global_position_start, int2_single(terrain_length - 1)), highway_ends, highway_ends_max_tunk);
        if (highways.waiting) {
            continue;
        }
        resize_TownMap(town_map, length * length);
        memset(town_map->value, 0, length * length);
        entity towns[zox_children_capacity];
        uint towns_length = zox_get_children_by_id(
            world,
            region,
            towns,
            zox_children_capacity,
            zox_id(Town));
        if (dbg_log >= 2) {
            zox_log("Towns found in Tunk [%ix%i]: [%i]",
                tunk_position->value.x,
                tunk_position->value.y,
                towns_length);
        }
        // a region with no towns still ends its tunks (a continue here left them generating forever, the rare
        //  terrain loading stall: chunks waited on their tunk, the player's spawn on them)
        for (int j = 0; j < towns_length; j++) {
            entity town = towns[j];
            int2 town_position = zox_getv(town, BlockPosition2);
            byte2 town_size = zox_getv(town, TownSize);
            // NOTE: Checks if Tunk is within Town Bounds
            int town_left_side = town_position.x - town_size.x;
            int town_right_side = town_position.x + town_size.x;
            int town_down_side = town_position.y - town_size.y;
            int town_up_side = town_position.y + town_size.y;
            if (town_left_side > global_position_start.x + terrain_length ||
                town_right_side < global_position_start.x ||
                town_down_side > global_position_start.y + terrain_length ||
                town_up_side < global_position_start.y
            ) {
                continue;
            }
            if (dbg_log) {
                zox_log("Town [%i] found in Tunk [%ix%i]: [%i]", j, tunk_position->value.x, tunk_position->value.y, towns_length);
            }
            // its bounds, its wall's curve inside them (fun/town.c)
            const int left = town_position.x - town_size.x / 2, right = town_position.x + town_size.x / 2;
            const int bottom = town_position.y - town_size.y / 2, top = town_position.y + town_size.y / 2;
            const TownShape shape = town_shape_of(world, town);
            // only the columns the town covers in this tunk, inside its wall's outer edge
            int2 position, from, to;
            if (!region_tunk_box(&rt, town_position, (int2) { town_size.x / 2 + 1, town_size.y / 2 + 1 }, &from, &to)) {
                continue;
            }
            for (position.x = from.x; position.x <= to.x; position.x++) {
                for (position.y = from.y; position.y <= to.y; position.y++) {
                    const int2 g = region_tunk_global(&rt, position);
                    const byte value = town_wall_value(&shape, g);
                    if (!value) {
                        continue;
                    }
                    const int index = int2_array_index(position, map_size);
                    town_map->value[index] = value;
                    // towns clear their ground of plants
                    if (!zox_disable_vegetation) {
                        vegetation_map->value[index] = 0;
                    }
                }
            }
            // homes: their parts on the map, the ground under them levelled to their floor with steps to their doors
            //  (fun/home.c, HomeFloor), doors any way (HomeFacing)
            //  - two passes: margins and steps, then the homes themselves, a home's floor wins over a neighbour's steps
            TownHome town_homes[town_homes_max];
            byte town_homes_count = 0;
            if (!zox_disable_homes) {
                entity homes[zox_children_capacity];
                const uint homes_length = zox_get_children_by_id(world, town, homes, zox_children_capacity, zox_id(Home));
                for (uint k = 0; k < homes_length && town_homes_count < town_homes_max; k++) {
                    town_homes[town_homes_count++] = town_home(world, homes[k]);
                }
            }
            for (byte inside_pass = 0; inside_pass < 2; inside_pass++)
            for (byte k = 0; k < town_homes_count; k++) {
                const TownHome h = town_homes[k];
                int2 home_from, home_to;
                const int2 reach = int2_single(int_max(h.half.x, h.half.y) + home_steps + 1);
                if (!region_tunk_box(&rt, h.center, reach, &home_from, &home_to)) {
                    continue;
                }
                for (position.x = home_from.x; position.x <= home_to.x; position.x++) {
                    for (position.y = home_from.y; position.y <= home_to.y; position.y++) {
                        const int2 g = region_tunk_global(&rt, position);
                        if (g.x < left || g.x > right || g.y < bottom || g.y > top) {
                            continue;
                        }
                        const byte inside = abs(g.x - h.center.x) <= h.half.x && abs(g.y - h.center.y) <= h.half.y;
                        if (inside != inside_pass) {
                            continue;
                        }
                        const int index = int2_array_index(position, map_size);
                        const int ground = home_ground_at(&h, g, height_map->value[index]);
                        if (ground >= 0) {
                            height_map->value[index] = ground;
                        }
                        if (inside) {
                            town_map->value[index] = zox_town_type_home;
                        }
                    }
                }
            }
            // its roads, round its homes (their margins and steps keep their ground), and its street lamps beside them
            //  (fun/town.c)
            const RoadSegments* roads = zox_get(town, RoadSegments);
            town_map_roads(&rt, roads->value, roads->length, town_road_half + 0.5f, zox_town_type_zone, zox_town_type_road,
                town_homes, town_homes_count, town_map, height_map, vegetation_map);
            const StreetLamps* lamps = zox_get(town, StreetLamps);
            for (int l = 0; l < lamps->length && rt.depth_difference == 1; l++) {
                const int2 p = int2_sub(lamps->value[l], rt.global_start);
                if (p.x >= 0 && p.y >= 0 && p.x < rt.length && p.y < rt.length &&
                    town_map->value[int2_array_index(p, map_size)] == zox_town_type_zone) {
                    town_map->value[int2_array_index(p, map_size)] = zox_town_type_lamp;
                }
            }
        }
        // the highways crossing it (fun/highways.c), on ground no town holds, level between their ends' heights
        if (highways_count) {
            TerrainHeights th;
            const byte has_heights = terrain_heights_begin(world, zox_get_parent(world, e), &th);
            const int sea = terrain_sea_height * render_distance_y;
            int3 segments[highway_ends_max_tunk];
            for (int j = 0; j < highways_count; j++) {
                segments[j] = (int3) { highway_ends[j].x, has_heights ? highway_ground(&th, sea, highway_ends[j]) : -1, highway_ends[j].y };
            }
            town_map_roads(&rt, segments, highways_count, highway_half + 0.5f, zox_town_type_none, zox_town_type_highway,
                NULL, 0, town_map, height_map, vegetation_map);
        }
        generate->value = zox_generate_tunk_end;
    }
    zox_sys_on_end();
} zoxd_system(town_map_system);
