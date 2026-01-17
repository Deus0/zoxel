byte disable_biomes = 1;
byte disable_top_placements = 0;
byte disable_dirt_patches = 1;
byte disable_grass_placements = 0;

// #define disable_newheightmap_gen

// place grass if max depth

// generates our terrain voxels
zox_sys2(GrassyPlainsSystem) {
    // zox_ts_begin(grassy_plains);
    const uint seed = global_seed;  // TODO: use terrains seed
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkPosition);
    zox_sys_in(RenderDepth);
    zox_sys_in(RenderDepthDirty);
    zox_sys_in(VoxelNodeEdited);
    zox_sys_in(VoxLink);
    zox_sys_out(VoxelNode);
    zox_sys_out(NodeDepth);
    zox_sys_out(VoxelNodeDirty);
    zox_sys_out(VoxelNodeGenerated);
    zox_sys_out(VoxelNodeLoaded);

    byte any_dirty = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDepthDirty, dirty)
        if (dirty->value == zox_dirty_active) {
            any_dirty = 1;
            break;
        }
    }
    if (!any_dirty) {
        return;
    }

    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDepth, render_depth);
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_i(RenderDepthDirty, dirty);
        zox_sys_i(VoxelNodeEdited, edited);
        zox_sys_i(VoxLink, terrain);
        zox_sys_o(NodeDepth, vdepth);
        zox_sys_o(VoxelNode, node);
        zox_sys_o(VoxelNodeDirty, node_dirty);
        zox_sys_o(VoxelNodeGenerated, generated);
        zox_sys_o(VoxelNodeLoaded, loaded);

        // todo: remember if has generated yet, keep a generated LOD state!
        //      - better yet just increase NodeDepth - and compare with terrain's one when increasing
        if (dirty->value != zox_dirty_active) {
            continue;
        }

        zox_geter_value(terrain->value, NodeDepth, byte, terrain_depth);
        byte generation_depth = optimize_generation_lods ? render_depth->value : terrain_depth;
        // if no depth, skip
        if (!generation_depth) {
            continue;
        }

        // kicks off lighting if loaded
        if (loaded->value && edited->value) {
            generated->value = zox_dirty_trigger;   // need kick start the lighting, even if loaded!
            continue;
        }

        // If Chunk already at or greater in Voxel Octree Resolution depth, we continue
        if (vdepth->value >= generation_depth) {
            // we should set to rebuild mesh still, with node dirty
            //  Until we cache those extra meshes
            node_dirty->value = zox_dirty_trigger;
            continue;
        }

        vdepth->value = generation_depth;
        const byte is_max_depth = vdepth->value == terrain_depth;

        const byte vlength = powers_of_two_byte[vdepth->value];
        const double terrain_amplifier = vlength * render_distance_y;
        const float2 map_size_f = float2_single(vlength);
        const float3 chunk_position_float3 = float3_from_int3(cposition->value);
        const int chunk_position_y = (int) (chunk_position_float3.y * vlength);
        byte3 positionl;
        // For each XZ position

        zox_geter(terrain->value, RealmLink, realm);
        zox_geter(realm->value, BiomeLinks, biomes);
        if (!biomes->length) {
            zox_log_error("No Biomes");
            continue;
        }

        // const entity biome = cposition->value.z > 0 ? biomes->value[0] :  biomes->value[biomes->length - 1];
        entity biome_1 = biomes->value[0];
        entity biome2 = biomes->value[1];

        zox_geter(biome_1, BlockLinks, biome_blocks_1);
        zox_geter(biome2, BlockLinks, biome_blocks_2);
        if (!biome_blocks_1->length || !biome_blocks_2->length) {
            zox_log_error("No Blocks in Biome [%s]", zox_get_name(biome_1));
            continue;
        }

        entity dirt_1 = biome_blocks_1->value[0];
        entity grass_1 = biome_blocks_1->value[1];
        entity dirt_2 = biome_blocks_2->value[0];
        entity grass_2 = biome_blocks_2->value[1];
        if (!zox_valid(dirt_1) || !zox_valid(grass_1) ||
            !zox_valid(dirt_2) || !zox_valid(grass_2)
        ) {
            zox_log_error("Dirt [%s] invalid in Biome [%s]", zox_get_name(biome_1));
            continue;
        }

        zox_geter_value(dirt_1, BlockIndex, byte, biome_dirt_id_1);
        zox_geter_value(grass_1, BlockIndex, byte, biome_grass_id_1);
        zox_geter_value(dirt_2, BlockIndex, byte, biome_dirt_id_2);
        zox_geter_value(grass_2, BlockIndex, byte, biome_grass_id_2);

        if (!biome_dirt_id_1) {
            zox_log_error("Biome dirt is air.");
            continue;
        }
        /*zox_log("   Block [%s] index [%i]", zox_get_name(dirt), biome_dirt_id);
        zox_log("   Block [%s] index [%i]", zox_get_name(grass), biome_grass_id);*/


        // ### Get Map Data ###
        // Gets the power multiplier for depth difference for our array lookups
        int hmultiplier = 1;
        byte ccc = vdepth->value;
        while (ccc != terrain_depth) {
            hmultiplier *= 2;
            ccc++;
        }

        int max_chunk_length = powers_of_two[terrain_depth];
        int2 hsize = int2_single(max_chunk_length);
        int2 cposition2 = (int2) { cposition->value.x, cposition->value.z };
        zox_geter(terrain->value, Chunk2Links, chunks2);

        entity chunk2 = int2_hashmap_get(chunks2->value, cposition2);
        if (!zox_valid(chunk2)) {
            zox_log_error("Invalid [chunk2] at [%ix%i] [y%i]", cposition2.x, cposition2.y, cposition->value.y);
            continue;
        }

        zox_geter(chunk2, BiomeMap, bmap);
        zox_geter(chunk2, HeightMap, hmap);
        zox_geter(chunk2, VegetationMap, vmap);
        if (!hmap->length || !bmap->length || !vmap->length) {
            zox_log_error("Invalid [HeightMap] at [%ix%i] [y%i]", cposition2.x, cposition2.y, cposition->value.y);
            continue;
        }

//#endif


        write_lock_VoxelNode(node);

        // Clear Octree to Air
        set_voxelt_air(node);
        /*for (positionl.x = 0; positionl.x < vlength; positionl.x++) {
            for (positionl.y = 0; positionl.y <= vlength; positionl.y++) {
                for (positionl.z = 0; positionl.z < vlength; positionl.z++) {
                    set_voxelt_override(node, vdepth->value, positionl, 0, 0);
                }
            }
        }*/

        for (positionl.x = 0; positionl.x < vlength; positionl.x++) {
            for (positionl.z = 0; positionl.z < vlength; positionl.z++) {

                const float2 positionn = (float2) {
                    noise_positiver2 + chunk_position_float3.x + (positionl.x / map_size_f.x),
                    noise_positiver2 + chunk_position_float3.z + (positionl.z / map_size_f.y)
                };

                byte is_mountain = 0;

                int2 hposition = (int2) {
                    positionl.x * hmultiplier,
                    positionl.z * hmultiplier
                };
                int hindex = int2_array_index(hposition, hsize);
                int global_position_y = (int) (hmap->value[hindex] - 128);
                global_position_y /= hmultiplier;
                byte biome_id = bmap->value[hindex];
                byte veggie = vmap->value[hindex];

                byte biome_dirt_id = biome_id == 0 ? biome_dirt_id_1 : biome_dirt_id_2;
                byte biome_grass_id = biome_id == 0 ? biome_grass_id_1 : biome_grass_id_2;

                // zox_log("- TerrainHeight [%ix%i] [%i]", hposition.x, hposition.y, global_position_y);

                const int local_height_raw = global_position_y - chunk_position_y;
                const int local_height = int_min(vlength - 1, local_height_raw);

                // byte did_place_grass_top = 0;
                // For each Y position
                if (local_height >= 0) {
                    for (positionl.y = 0; positionl.y <= local_height; positionl.y++) {
                        const int place_positionv = chunk_position_y + positionl.y;
                        // top blocks
                        byte value;
                        if (cposition->value.y == -render_distance_y && positionl.y == 0) {
                            value = zox_block_obsidian;
                        } else if (!disable_top_placements && positionl.y  == local_height_raw) {
                            if (is_mountain) {
                                value = zox_block_stone;
                            } else {

                                if (global_position_y < sand_height) {
                                    value = zox_block_sand;
                                } else if (global_position_y == sand_height) {
                                    value = biome_dirt_id;
                                } else {
                                    value = veggie ? biome_grass_id : biome_dirt_id;
                                    // did_place_grass_top = is_plant_grass;

                                    /*if (!disable_dirt_patches) {
                                        value = biome_grass_id;
                                        did_place_grass_top = 1;
                                    } else {
                                        const double place_value = (perlin_octaves(
                                            positionn.x,
                                            positionn.y,
                                            0.16,
                                            seed * 2,
                                            4) + 1.0) / 2.0;
                                        if (place_value <= 0.73) {
                                            value = biome_grass_id;
                                            did_place_grass_top = 1;
                                        } else {
                                            value = biome_dirt_id;
                                        }
                                    }*/
                                }
                            }
                            // TODO: Check Global
                        } else if (place_positionv == global_position_y - 1) {
                            value = biome_dirt_id;
                        } else {
                            value = zox_block_stone;
                        }

                        set_voxelt(node, vdepth->value, positionl, value, 0);
                    }
                }

                // Place Grass on tops
                if (!disable_grass_placements &&
                    !disable_block_vox_generation &&
                    !is_mountain &&
                    is_max_depth && // if max depth
                    veggie == 2 &&
                    local_height_raw + 1 >= 0 &&
                    local_height_raw + 1 < vlength &&
                    global_position_y > sand_height
                ) {
                    // byte place_grass =
                    /*should_place_grass(
                        positionn.x,
                        positionn.y,
                        seed);*/
                    positionl.y = local_height_raw + 1;
                    set_voxelt(node, vdepth->value, positionl, zox_block_vox_grass, 0);
                }
            }
        }
        write_unlock_VoxelNode(node);

        node_dirty->value = zox_dirty_trigger;
        generated->value = zox_dirty_trigger;
        loaded->value = 1;
    }
} zox_sys_end(GrassyPlainsSystem);



/*const double place_value = (perlin_octaves(
    n oise_position_x, *
    noise_position_z,
    0.216,
    seed * 16,
    4) + 1.0) / 2.0;
    const double place_value2 = (perlin_octaves(
        noise_position_x,
        noise_position_z,
        0.616,
        seed * 16,
        6) + 1.0) / 2.0;*/
// place_value + place_value2 * 0.1 <= 0.33) {

/*const int rando = rand() % 10000;
if (rando <= block_spawn_chance_grass + block_spawn_chance_flower + block_spawn_chance_rubble) {
    positionl.y = local_height_raw + 1;
    data.position = positionl;
    if (rando <= block_spawn_chance_grass) {
        set_voxel(&datam_grass, data);
    }
    else if (rando <= block_spawn_chance_grass + block_spawn_chance_flower) {
        set_voxel(&datam_flower, data);
    }
    else if (rando <= block_spawn_chance_grass + block_spawn_chance_flower + block_spawn_chance_rubble) {
        set_voxel(&datam_rubble, data);
    }
    // zox_log(" + flower spawned %f\n", perlin_value)
}*/

// obisidian bottom
/*if (cposition->value.y == -render_distance_y) {
    positionl.y = 0;
    const int obsidian_height = rand() % terrain_obsidian_height;
    for (positionl.y = 0; positionl.y <= obsidian_height; positionl.y++) {
        // set_voxel(setter_obsidian, data);
        set_voxelt(
            node,
            vdepth,
            positionl,
            zox_block_obsidian,
            0);
    }
}*/



                /*if (!disable_biomes) {
                    const double mountain_noise = (perlin_octaves(
                        positionn.x,
                        positionn.y,
                        0.002,
                        seed,
                        3) + 1.0) / 2.0;
                        is_mountain = mountain_noise >= 0.6; // biome == zox_biome_mountain
                }*/

                // our height calc
/*#ifdef disable_newheightmap_gen
                const double mountain_amplifier = 2;
                const double height_frequency = is_mountain ? terrain_frequency * mountain_amplifier : terrain_frequency;
                const double height_amplifier = is_mountain ? terrain_amplifier * mountain_amplifier : terrain_amplifier;

                double perlin_value = perlin_octaves(
                    positionn.x,
                    positionn.y,
                    height_frequency,
                    seed,
                    terrain_octaves
                );
                perlin_value *= height_amplifier;
                const int global_position_y = int_floorf(perlin_value);
#else*/