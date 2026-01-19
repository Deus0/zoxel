zox_sys2(TownMapSystem) {
    // TODO: use terrains seed
    // TODO: use height frequency from biome maps
    // uint seed = global_seed;

    // TODO: Get these from Region Data
    int2 tposition = (int2) { 0, - 144 };
    int2 tsize = (int2) { 64, 64 };
    int gate_width = 4;

    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Chunk2Position);
    zox_sys_in(BiomeMap);
    zox_sys_out(TownMap);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(Chunk2Position, cposition);
        zox_sys_i(BiomeMap, bmap);
        zox_sys_o(TownMap, tmap);

        if (generate->value != zox_dirty_active) {
            continue;
        }

        if (!bmap->length) {
            zox_logw("[vmap] bmap map wasn't generated in time");
            continue;
        }

        // now generate heights
        int max_chunk_length = powers_of_two[terrain_depth];
        int2 hsize = int2_single(max_chunk_length);

        if (!tmap->value) {
            initialize_TownMap(tmap, hsize.x * hsize.y);
        }

        int2 lposition = int2_zero;
        int2 gposition_start = (int2) {
            cposition->value.x * hsize.x,
            cposition->value.y * hsize.y
        };
        int2 gposition = gposition_start;


        for (lposition.x = 0; lposition.x < hsize.x; lposition.x++, gposition.x++) {
            gposition.y = gposition_start.y;
            for (lposition.y = 0; lposition.y < hsize.y; lposition.y++, gposition.y++) {

                int index = int2_array_index(lposition, hsize);

                // Get Biome Data
                byte biome = bmap->value[index];

                byte value = 0;

                byte town_in_x = (gposition.x >= tposition.x - tsize.x / 2 && gposition.x <= tposition.x + tsize.x / 2);

                byte town_in_y = (gposition.y >= tposition.y - tsize.y / 2 &&
                    gposition.y <= tposition.y + tsize.y / 2);

                byte left_wall = gposition.x == tposition.x - tsize.x / 2 && town_in_y;

                byte right_wall = gposition.x == tposition.x + tsize.x / 2 && town_in_y;

                byte back_wall = gposition.y == tposition.y - tsize.y / 2 && town_in_x;

                // Gate is excluded from front wall
                byte town_gate_in_x = (gposition.x >= tposition.x - gate_width / 2 && gposition.x <= tposition.x + gate_width / 2);

                byte front_gate = gposition.y == tposition.y + tsize.y / 2 && town_gate_in_x;

                byte front_wall = gposition.y == tposition.y + tsize.y / 2 && town_in_x && !front_gate;

                if (back_wall || front_wall || left_wall || right_wall) {
                    value = 1;
                }

                tmap->value[index] = value;

                /*if (value) {
                    zox_log("townmap - value: %f", value);
                }*/
            }
        }
    }
} zox_sys_end(TownMapSystem);