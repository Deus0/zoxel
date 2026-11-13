// NOTE: Adds height to the heightmap using region mountains
void mountain_map_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TunkLod);
    zox_sys_in(TunkPosition);
    zox_sys_out(GenerateTunk);
    zox_sys_out(VegetationMap);
    zox_sys_out(HeightMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TunkLod, lod);
        zox_sys_i(TunkPosition, tunk_position);
        zox_sys_o(GenerateTunk, generate);
        zox_sys_o(VegetationMap, vegetation_map);
        zox_sys_o(HeightMap, height_map);
        if (generate->value != zox_generate_tunk_mountains) {
            continue;
        }
        if (zox_disable_mountains || zox_flatlands) {
            generate->value = zox_generate_tunk_towns;
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
        if (!height_map->length) {
            zox_logw("Invalid [height_map] in mountain_map_system");
            continue;
        }
        if (!zox_disable_vegetation && !vegetation_map->length) {
            zox_logw("Invalid [vegetation_map] in mountain_map_system");
            continue;
        }
#endif
        entity mountains[zox_children_capacity];
        uint mountains_length = zox_get_children_by_id(
            world,
            region,
            mountains,
            zox_children_capacity,
            zox_id(Mountain));
        if (dbg_log >= 2) {
            zox_log("Towns found in Tunk [%ix%i]: [%i]",
                tunk_position->value.x,
                tunk_position->value.y,
                mountains_length);
        }
        for (int j = 0; j < mountains_length; j++) {
            entity e2 = mountains[j];
            byte mountain_type = zox_getv(e2, MountainType);
            int2 mountain_position = zox_getv(e2, BlockPosition2);
            byte mountain_radius = zox_getv(e2, Radius);
            byte mountain_height = zox_getv(e2, Height);
            // only the columns the mountain covers in this tunk
            int2 position, from, to;
            if (!region_tunk_box(&rt, mountain_position, int2_single(mountain_radius), &from, &to)) {
                continue;
            }
            for (position.x = from.x; position.x <= to.x; position.x++) {
                for (position.y = from.y; position.y <= to.y; position.y++) {
                    const int2 global_position = region_tunk_global(&rt, position);
                    int mountain_distance = int2_distance(global_position, mountain_position);
                    if (mountain_distance > mountain_radius) {
                        continue;
                    }
                    int index = int2_array_index(position, map_size);
                    int value = height_map->value[index];
                        if (mountain_type == zox_mountain_type_peak) {
                        double mountain_multiplier = 1 + mountain_height * ((mountain_radius - mountain_distance) / (float) mountain_radius);
                        value *= mountain_multiplier;
                    } else {
                        // if type is zox_mountain_type_hill
                        float t = 1.0f - (float) mountain_distance / mountain_radius;
                        t = t * t * (3.0f - 2.0f * t); // smoothstep
                        value += (int)(mountain_height * t);
                    }
                    height_map->value[index] = int_clamp(value, 0, render_distance_y * terrain_length - 1);
                    // Clear Vegetation for middle of mountains
                    if (!zox_disable_vegetation && mountain_distance < 2 / (mountain_radius * 3)) {
                        vegetation_map->value[index] = 0;
                    }
                }
            }
        }
        generate->value = zox_generate_tunk_towns;
    }
    zox_sys_on_end();
} zoxd_system(mountain_map_system);
