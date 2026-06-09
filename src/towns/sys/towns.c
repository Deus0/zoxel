// NOTE: This builds out Simple Town Data into 2D Maps!
zox_sys2(TownMapSystem) {
    byte dbg_log = 0;
    // TODO: use terrains seed
    // TODO: use height frequency from biome maps
    // uint seed = global_seed;
    // TODO: Get these from Region Data
    // int2 town_position = (int2) { 0, - 80 };
    // int2 town_size = (int2) { 52, 52 };
    int gate_width = 4;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(RegionLink);
    zox_sys_in(Chunk2Position);
    zox_sys_in(BiomeMap);
    zox_sys_out(TownMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(RegionLink, region);
        zox_sys_i(Chunk2Position, cposition);
        zox_sys_i(BiomeMap, bmap);
        zox_sys_o(TownMap, tmap);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(region->value)) {
            zox_loge("Tunk has invalid region at [%ix%i]", cposition->value.x, cposition->value.y);
            continue;
        }
        if (!bmap->length) {
            zox_logw("[vmap] bmap map wasn't generated in time");
            continue;
        }
        entity towns[16];
        uint towns_length = zox_get_children_by_id(world, region->value, towns, 16, zox_id(Town));
        if (dbg_log >= 2) {
            zox_log("Towns found in Tunk [%ix%i]: [%i]", cposition->value.x, cposition->value.y, towns_length);
        }
        // now generate heights
        int max_chunk_length = powers_of_two[terrain_depth];
        int2 hsize = int2_single(max_chunk_length);
        if (!tmap->value) {
            initialize_TownMap(tmap, hsize.x * hsize.y);
            for (int j = 0; j < tmap->length; j++) {
                tmap->value[j] = 0;
            }
        }
        if (!towns_length) {
            continue;
        }
        entity town = towns[0];
        int3 town_voxel_position = zox_getv(town, VoxelPosition);
        int3 town_voxel_size = zox_getv(town, VoxelSize);
        int2 town_position = (int2) { town_voxel_position.x, town_voxel_position.z };
        int2 town_size = (int2) { town_voxel_size.x, town_voxel_size.z };
        int2 lposition = int2_zero;
        int2 gposition_start = (int2) {
            cposition->value.x * hsize.x,
            cposition->value.y * hsize.y
        };
        // NOTE: Checks if Tunk is within Town Bounds
        int town_left_side = town_position.x - town_size.x;
        int town_right_side = town_position.x + town_size.x;
        int town_down_side = town_position.y - town_size.y;
        int town_up_side = town_position.y + town_size.y;
        if (town_left_side > gposition_start.x + hsize.x ||
            town_right_side < gposition_start.x ||
            town_down_side > gposition_start.y + hsize.y ||
            town_up_side < gposition_start.y
        ) {
            continue;
        }
        if (dbg_log) {
            zox_log("Town found in Tunk [%ix%i]: [%i]", cposition->value.x, cposition->value.y, towns_length);
        }
        int2 gposition = gposition_start;
        for (lposition.x = 0; lposition.x < hsize.x; lposition.x++, gposition.x++) {
            gposition.y = gposition_start.y;
            for (lposition.y = 0; lposition.y < hsize.y; lposition.y++, gposition.y++) {
                int index = int2_array_index(lposition, hsize);
                // Get Biome Data
                // byte biome = bmap->value[index];
                byte value = 0;
                byte town_in_x = (gposition.x >= town_position.x - town_size.x / 2 && gposition.x <= town_position.x + town_size.x / 2);
                byte town_in_y = (gposition.y >= town_position.y - town_size.y / 2 && gposition.y <= town_position.y + town_size.y / 2);
                byte left_wall = gposition.x == town_position.x - town_size.x / 2 && town_in_y;
                byte right_wall = gposition.x == town_position.x + town_size.x / 2 && town_in_y;
                byte back_wall = gposition.y == town_position.y - town_size.y / 2 && town_in_x;
                // Gate is excluded from front wall
                byte town_gate_in_x = (gposition.x >= town_position.x - gate_width / 2 && gposition.x <= town_position.x + gate_width / 2);
                byte front_gate = gposition.y == town_position.y + town_size.y / 2 && town_gate_in_x;
                byte front_wall = gposition.y == town_position.y + town_size.y / 2 && town_in_x && !front_gate;
                if (back_wall || front_wall || left_wall || right_wall) {
                    value = 1;
                }
                if (value) {
                    tmap->value[index] = value;
                    if (dbg_log) {
                        zox_log(" + Town Wall [%i] at [%ix%i]", value, gposition.x, gposition.y);
                    }
                }
            }
        }
    }
} zox_sys_end(TownMapSystem);
