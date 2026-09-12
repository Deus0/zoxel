// NOTE: This builds out Simple Town Data into 2D Maps!
zox_sys2(TownMapSystem) {
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
        entity region = zox_get_link(world, e, RegionLink);
#ifdef zox_safety_checks
        if (!zox_valid(region)) {
            zox_loge("[%s] Tunk has invalid region at [%ix%i]: %lu in TownMaps", zox_get_name(e), tunk_position->value.x, tunk_position->value.y, region);
            continue;
        }
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
        // Generate Town Maps
        entity terrain = zox_get_parent(world, e);
        byte length = octree_size(lod->value);
        int2 map_size = int2_single(length);
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        byte terrain_length = octree_size(terrain_depth);
        byte depth_difference = octree_size(terrain_depth - lod->value);
        int2 global_position_start = (int2) {
            tunk_position->value.x * terrain_length,
            tunk_position->value.y * terrain_length
        };
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
        if (!towns_length) {
            continue;
        }
        for (int j = 0; j < towns_length; j++) {
            entity town = towns[j];
            int2 town_position = zox_getv(town, BlockPosition2);
            byte2 town_size = zox_getv(town, TownSize);
            byte wall_thickness = zox_getv(town, WallThickness);
            byte gate_width = 4;
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
            byte spike_spacing = 3;
            // Town bounds
            int left   = town_position.x - town_size.x / 2;
            int right  = town_position.x + town_size.x / 2;
            int bottom = town_position.y - town_size.y / 2;
            int top    = town_position.y + town_size.y / 2;
            //uint town_heights_total = 0;
            // uint town_blocks_count = 0;
            int2 global_position = global_position_start;
            int2 position;
            for (position.x = 0; position.x < length; position.x++, global_position.x += depth_difference) {
                global_position.y = global_position_start.y;
                for (position.y = 0; position.y < length; position.y++, global_position.y += depth_difference) {
                    // NOTE: Skip if not in town
                    if (!(global_position.x >= left &&
                        global_position.x <= right &&
                        global_position.y >= bottom &&
                        global_position.y <= top)) {
                        continue;
                    }
                    int index = int2_array_index(position, map_size);
                    byte value;
                    // NOTE: 1 is inside town!
                    /*byte town_in_x = (global_position.x >= town_position.x - town_size.x / 2 && global_position.x <= town_position.x + town_size.x / 2);
                    byte town_in_y = (global_position.y >= town_position.y - town_size.y / 2 && global_position.y <= town_position.y + town_size.y / 2);
                    byte left_wall = global_position.x >= town_position.x - town_size.x / 2 && global_position.x <= town_position.x - town_size.x / 2 + wall_thickness && town_in_y;
                    byte right_wall = global_position.x >= town_position.x + town_size.x / 2 - wall_thickness && global_position.x <= town_position.x + town_size.x / 2 && town_in_y;
                    byte back_wall = global_position.y >= town_position.y - town_size.y / 2 && global_position.y <= town_position.y - town_size.y / 2 + wall_thickness && town_in_x;
                    // Gate is excluded from front wall
                    byte town_gate_in_x = (global_position.x >= town_position.x - gate_width / 2 && global_position.x <= town_position.x + gate_width / 2);
                    byte front_wall = global_position.y >= town_position.y + town_size.y / 2 - wall_thickness && global_position.y <= town_position.y + town_size.y / 2 && town_in_x;
                    byte front_gate = front_wall && town_gate_in_x;
                    front_wall &= !front_gate;
                    if (back_wall || front_wall || left_wall || right_wall) {
                        // byte is_spike = ((left_wall || right_wall) && is_odd(position.x)) || ((back_wall || front_wall) && is_odd(position.y));
                        // value = is_spike ? zox_town_type_wall_spike : zox_town_type_wall;
                    } else if (front_gate) {
                        value = zox_town_type_gate;
                    } else {
                        value = zox_town_type_zone;
                    }*/
                    // Wall bands with thickness
                    byte left_wall   = global_position.x >= left && global_position.x < left + wall_thickness && global_position.y >= bottom && global_position.y <= top;
                    byte right_wall  = global_position.x <= right && global_position.x > right - wall_thickness && global_position.y >= bottom && global_position.y <= top;
                    byte back_wall   = global_position.y >= bottom && global_position.y < bottom + wall_thickness && global_position.x >= left && global_position.x <= right;
                    byte front_wall  = global_position.y <= top && global_position.y > top - wall_thickness && global_position.x >= left && global_position.x <= right;
                    // Gate is excluded from front wall
                    byte town_gate_in_x = global_position.x >= town_position.x - gate_width / 2 && global_position.x <= town_position.x + gate_width / 2;
                    byte front_gate = front_wall && town_gate_in_x;
                    // front_wall &= !front_gate;
                    // Corners of the wall band
                    byte bottom_left_corner =
                    global_position.x >= left && global_position.x < left + wall_thickness &&
                    global_position.y >= bottom && global_position.y < bottom + wall_thickness;
                    byte bottom_right_corner = global_position.x > right - wall_thickness && global_position.x <= right &&
                    global_position.y >= bottom && global_position.y < bottom + wall_thickness;
                    byte top_left_corner =
                    global_position.x >= left && global_position.x < left + wall_thickness &&
                    global_position.y > top - wall_thickness && global_position.y <= top;
                    byte top_right_corner =
                    global_position.x > right - wall_thickness && global_position.x <= right && global_position.y > top - wall_thickness && global_position.y <= top;
                    byte is_corner = bottom_left_corner || bottom_right_corner || top_left_corner || top_right_corner;
                    // ### NOTE: Here we define the wall edges ###
                    byte left_outer_edge = left_wall && global_position.x == left;
                    byte left_inner_edge = left_wall && global_position.x == left + wall_thickness - 1 && !is_corner;
                    byte right_outer_edge = right_wall && global_position.x == right;
                    byte right_inner_edge = right_wall && global_position.x == right - wall_thickness + 1 && !is_corner;
                    byte bottom_outer_edge = back_wall && global_position.y == bottom;
                    byte bottom_inner_edge = back_wall && global_position.y == bottom + wall_thickness - 1 && !is_corner;
                    byte top_outer_edge = front_wall && global_position.y == top;
                    byte top_inner_edge = front_wall && global_position.y == top - wall_thickness + 1 && !is_corner;
                    byte is_edge = left_outer_edge || left_inner_edge || right_outer_edge || right_inner_edge || bottom_outer_edge || bottom_inner_edge || top_outer_edge || top_inner_edge;
                    // ### NOTE: Adds Castle Wall Spikes ###
                    // byte left_edge = left_wall && (global_position.x == left || global_position.x == left + wall_thickness - 1);
                    // byte right_edge = right_wall && (global_position.x == right || global_position.x == right - wall_thickness + 1);
                    // byte back_edge = back_wall && (global_position.y == bottom || global_position.y == bottom + wall_thickness - 1);
                    // byte front_edge = front_wall && (global_position.y == top || global_position.y == top - wall_thickness + 1);
                    byte spike_x = (byte)(abs(global_position.x - left) % spike_spacing == 0);
                    byte spike_y = (byte)(abs(global_position.y - bottom) % spike_spacing == 0);
                    // byte is_edge = left_edge || right_edge || back_edge || front_edge;
                    // byte is_spike = ((left_edge || right_edge) && spike_y) || ((back_edge || front_edge) && spike_x);
                    byte is_spike = ((left_outer_edge || left_inner_edge || right_outer_edge || right_inner_edge) && spike_y) ||
                    ((bottom_outer_edge || bottom_inner_edge || top_outer_edge || top_inner_edge) && spike_x);
                    // ### End Spikes ###
                    /*if (is_corner) {
                        value = zox_town_type_wall_spike;
                    } else */
                    if (front_gate) {
                        value = zox_town_type_gate;
                    } else if (is_spike) {
                        value = zox_town_type_wall_spike;
                    } else if (is_edge) {
                        value = zox_town_type_wall_edge;
                    } else if (left_wall || right_wall || back_wall || front_wall) {
                        value = zox_town_type_wall;
                    } else {
                        value = zox_town_type_zone;
                    }
                    town_map->value[index] = value;
                    // NOTE: Towns remove all vegetation!
                    if (!zox_disable_vegetation) {
                        vegetation_map->value[index] = 0;
                    }
                    if (dbg_log) {
                        zox_log(" + Town Wall [%i] at [%ix%i]", value, global_position.x, global_position.y);
                    }
                }
            }
            // NOTE: Averages out town based on distance to its center
            // Townwards town center, make height closer to average
            // TODO: Completely flatten out in min_dim/2 centre - half radius
            /*int min_dim = int_min(town_size.x, town_size.y);
            uint town_height_target = (byte) (town_heights_total / town_blocks_count);
            global_position = global_position_start;
            for (lposition.x = 0; lposition.x < map_size.x; lposition.x++, global_position.x++) {
                global_position.y = global_position_start.y;
                for (lposition.y = 0; lposition.y < map_size.y; lposition.y++, global_position.y++) {
                    // NOTE: Skip if not in town
                    if (!(global_position.x >= town_position.x - town_size.x / 2 &&
                        global_position.x <= town_position.x + town_size.x / 2 &&
                        global_position.y >= town_position.y - town_size.y / 2 &&
                        global_position.y <= town_position.y + town_size.y / 2)) {
                        continue;
                    }
                    int index = int2_array_index(lposition, map_size);
                    // get distance to middle
                    int distance = int2_distance(global_position, town_position);
                    float distancef = distance / (float) min_dim;
                    // lerp the value based off distance
                    if (is_effect_heights) {
                        height_map->value[index] = (distancef) * height_map->value[index] + (1 - distancef) * town_height_target;
                    }
                }
            }*/
            if (zox_disable_homes) {
                continue;
            }
            entity homes[zox_children_capacity];
            uint homes_length = zox_get_children_by_id(world, town, homes, zox_children_capacity, zox_id(Home));
            if (dbg_log) {
                zox_log(" - Town Homes [%i]", homes_length);
            }
            // Place Homes in map
            for (int k = 0; k < homes_length; k++) {
                entity home = homes[k];
                int2 home_position = zox_getv(home, BlockPosition2);
                byte2 home_size = zox_getv(home, HomeSize);
                byte door_width = 2;
                // home_height = ; byte wall_thickness = zox_getv(town, WallThickness);
                // NOTE: For each position in chunk, XZ, we check if inside Home
                int2 position;
                global_position = global_position_start;
                for (position.x = 0; position.x < length; position.x++, global_position.x += depth_difference) {
                    global_position.y = global_position_start.y;
                    for (position.y = 0; position.y < length; position.y++, global_position.y += depth_difference) {
                        // NOTE: Skip if not in town
                        if (!(global_position.x >= town_position.x - town_size.x / 2 &&
                            global_position.x <= town_position.x + town_size.x / 2 &&
                            global_position.y >= town_position.y - town_size.y / 2 &&
                            global_position.y <= town_position.y + town_size.y / 2)) {
                            continue;
                        }
                        int index = int2_array_index(position, map_size);
                        // NOTE: Skips if not in home
                        if (!(global_position.x >= home_position.x - home_size.x / 2 &&
                            global_position.x <= home_position.x + home_size.x / 2 &&
                            global_position.y >= home_position.y - home_size.y / 2 &&
                            global_position.y <= home_position.y + home_size.y / 2)) {
                            continue;
                        }
                        // NOTE: 1 is inside town!
                        byte left_wall = global_position.x == home_position.x - home_size.x / 2;
                        byte right_wall = global_position.x == home_position.x + home_size.x / 2;
                        byte back_wall = global_position.y == home_position.y - home_size.y / 2;
                        byte front_wall = global_position.y == home_position.y + home_size.y / 2;
                        // Gate is excluded from front wall
                        byte in_door_x = (global_position.x >= home_position.x - door_width / 2 && global_position.x <= home_position.x + door_width / 2);
                        byte is_door = front_wall && in_door_x;
                        byte value;
                        if (is_door) {
                            value = zox_town_type_home_door;
                        } else if (back_wall || front_wall || left_wall || right_wall) {
                            value = zox_town_type_home_wall;
                        } else {
                            value = zox_town_type_home;
                        }
                        town_map->value[index] = value;
                        /*if (is_effect_heights) {
                            height_map->value[index] = town_height_target;
                        }*/
                    }
                }
            }
        }
        generate->value = zox_generate_tunk_end;
    }
} zox_sys_end(TownMapSystem);
