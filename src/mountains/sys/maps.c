// NOTE: Adds height to the heightmap using region mountains
zox_sys2(MountainMapSystem) {
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
        entity region = zox_get_link(world, e, Region);
#ifdef zox_safety_checks
        if (!zox_valid(region)) {
            zox_loge("[%s] Tunk has invalid region at [%ix%i]: %lu in MountainMaps",
                zox_get_name(e),
                tunk_position->value.x,
                tunk_position->value.y,
                region);
            continue;
        }
#endif
        // NOTE: Skip if still Generating Region
        if (zox_getv(region, GenerateRegion)) {
            continue;
        }
#ifdef zox_safety_checks
        if (!height_map->length) {
            zox_logw("Invalid [height_map] in MountainMapSystem");
            continue;
        }
        if (!zox_disable_vegetation && !vegetation_map->length) {
            zox_logw("Invalid [vegetation_map] in MountainMapSystem");
            continue;
        }
#endif
        entity terrain = zox_get_parent(world, e);
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        byte terrain_length = octree_size(terrain_depth);
        byte length = octree_size(lod->value);
        int2 map_size = int2_single(length);
        byte depth_difference = octree_size(terrain_depth - lod->value);
        int2 global_position_start = (int2) {
            tunk_position->value.x * terrain_length,
            tunk_position->value.y * terrain_length
        };
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
            int2 global_position = global_position_start;
            int2 position;
            for (position.x = 0; position.x < length; position.x++, global_position.x += depth_difference) {
                global_position.y = global_position_start.y;
                for (position.y = 0; position.y < length; position.y++, global_position.y += depth_difference) {
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
} zox_sys_end(MountainMapSystem);
