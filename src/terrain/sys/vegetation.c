// NOTE: Places vegetation, only in top chunks
zox_sys2(VegetationChunk3System) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkPosition);
    zox_sys_in(VoxLink);
    zox_sys_in(TunkLink);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, state);
        zox_sys_i(NodeDepth, voctree_depth);
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_i(VoxLink, terrain);
        zox_sys_i(TunkLink, tunk);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(VoxelNodeDirty, dirty);
        if (state->value != zox_dirty_end) { // zox_dirty_active) {
            continue;
        }
        zox_geter_value(terrain->value, RealmLink, entity, realm);
        if (!zox_valid(realm)) {
            continue;
        }
        zox_geter(realm, BiomeLinks, realm_biomes);
        if (!realm_biomes->length) {
            zox_log_error("No Biomes on Realm");
            continue;
        }
        zox_geter_value(terrain->value, NodeDepth, byte, terrain_depth);
        byte is_max_depth = voctree_depth->value == terrain_depth;
        if (!is_max_depth) {
            continue;
        }
        byte voctree_length = powers_of_two_byte[voctree_depth->value];
        int chunk_position_y = cposition->value.y * voctree_length;
        byte3 positionl;
        int hmultiplier = 1;
        byte ccc = voctree_depth->value;
        while (ccc != terrain_depth) {
            hmultiplier *= 2;
            ccc++;
        }
        int max_chunk_length = powers_of_two[terrain_depth];
        int2 hsize = int2_single(max_chunk_length);
        if (!zox_valid(tunk->value)) {
            zox_log_error("Invalid [Tunk] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
        zox_geter(tunk->value, HeightMap, height_map);
        zox_geter(tunk->value, BiomeMap, biome_map);
        zox_geter(tunk->value, VegetationMap, vegetation_map);
        if (!vegetation_map->length) {
            zox_log_error("Invalid [Tunk] [Maps] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
        entity biome = 0;
        byte soil_grass_id = 0;
        byte grass_id = 0;
        byte wood_id = 0;
        write_lock_VoxelNode(voctree);
        for (positionl.x = 0; positionl.x < voctree_length; positionl.x++) {
            for (positionl.z = 0; positionl.z < voctree_length; positionl.z++) {
                int2 hposition = (int2) { positionl.x * hmultiplier, positionl.z * hmultiplier };
                int map_index = int2_array_index(hposition, hsize);
                byte biome_id = biome_map->value[map_index];
                byte veggie = vegetation_map->value[map_index];
                byte height = height_map->value[map_index];
                int terrain_top_position = (int) height;
                int top_position = terrain_top_position - chunk_position_y;
                top_position /= hmultiplier;
                if (top_position < 0 || top_position >= voctree_length) {
                    continue;
                }
                // NOTE: Checks if outer bounds to determine if on top of world
                // byte is_top_chunk = top_position < voctree_length;
                // top_position = int_clamp(top_position, 0, voctree_length - 1);
                if (biome_id >= realm_biomes->length) {
                    zox_loge("Biome ID OOB [%i] of [%i]", biome_id, realm_biomes->length);
                    continue;
                }
                entity target_biome = realm_biomes->value[biome_id];
                if (!zox_valid(target_biome)) {
                    zox_loge("Biome is invalid [%i]", biome_id);
                    continue;
                }
                // NOTE: Updates our cache of our biome
                if (biome != target_biome) {
                    biome = target_biome;
                    // Get our biome vegetation blocks
                    entity soil_grass = zox_get_child_by_id(world, biome, zox_id(BlockSoilGrass));
                    entity grass = zox_get_child_by_id(world, biome, zox_id(BlockGrass));
                    entity wood = zox_get_child_by_id(world, biome, zox_id(BlockWood));
                    // Get IDs from Entitites
                    soil_grass_id = zox_valid(soil_grass) ? zox_getv(soil_grass, BlockIndex) : 0;
                    grass_id = zox_valid(grass) ? zox_getv(grass, BlockIndex) : 0;
                    wood_id = zox_valid(wood) ? zox_getv(wood, BlockIndex) : 0;
                }
                // ### Only continue if can place ###
                if (veggie == 1) {
                    // NOTE: Make dirt Soil Grass
                    positionl.y = top_position;
                    set_VoxelNode(voctree, voctree_depth->value, positionl, soil_grass_id, 0);
                } else if (veggie == 2) {
                    // Place Grass on tops
                    if (grass_id) {
                        positionl.y = top_position + 1;
                        if (positionl.y >= 0 && positionl.y < voctree_length) {
                            if (!getv_VoxelNode(voctree, positionl, voctree_depth->value)) {
                                set_VoxelNode(voctree, voctree_depth->value, positionl, grass_id, 0);
                            }
                        }
                    }
                } else if (veggie == 3) {
                    // TODO: Grow through chunks neighbors
                    // Trees
                    for (int h = 1; h <= 2 + rand() % 4; h++) {
                        positionl.y = top_position + h;
                        if (positionl.y >= 0 && positionl.y < voctree_length) {
                            set_VoxelNode(voctree, voctree_depth->value, positionl, wood_id, 0);
                        }
                    }
                }
            }
        }
        write_unlock_VoxelNode(voctree);
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(VegetationChunk3System);




        // todo: remember if has generated yet, keep a generated LOD state!
        //      - better yet just increase NodeDepth - and compare with terrain's one when increasi ng
        /*if (dirty->value != zox_dirty_active) {
            continue;
        }
        zox_geter_value(terrain->value, NodeDepth, byte, terrain_depth);
        byte generation_depth = !zox_dbg_disable_generation_lods ? render_depth->value : terrain_depth;
        if (zox_dbg_boost_generation_depth && generation_depth != terrain_depth) {
            generation_depth++;
        }
        // if no depth, skip
        if (!generation_depth) {
            continue;
        }
        // kicks off lighting if loaded
        if (loaded->value && edited->value) {
            generated->value = zox_dirty_trigger;   // need kick start the lighting, even if loaded!
            // still need this
            if (voctree_depth->value >= generation_depth) {
                vdirty->value = zox_dirty_trigger;
            }
            continue;
        }
        // If Chunk already at or greater in Voxel Octree Resolution depth, we continue
        if (voctree_depth->value >= generation_depth) {
            // we should set to rebuild mesh still, with voctree dirty
            //  Until we cache those extra meshes
            vdirty->value = zox_dirty_trigger;
            continue;
        }
        voctree_depth->value = generation_depth;
        byte is_max_depth = voctree_depth->value == terrain_depth;
        byte voctree_length = powers_of_two_byte[voctree_depth->value];
        float3 chunk_position_float3 = float3_from_int3(cposition->value);
        int chunk_position_y = (int) (chunk_position_float3.y * voctree_length);
        byte3 positionl;
        // For each XZ position
        // ### Get Map Data ###
        // Gets the power multiplier for depth difference for our array lookups
        int hmultiplier = 1;
        byte ccc = voctree_depth->value;
        while (ccc != terrain_depth) {
            hmultiplier *= 2;
            ccc++;
        }
        int max_chunk_length = powers_of_two[terrain_depth];
        int2 hsize = int2_single(max_chunk_length);
        if (!zox_valid(tunk->value)) {
            zox_log_error("Invalid [Tunk] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
        zox_geter(tunk->value, BiomeMap, biome_map);
        zox_geter(tunk->value, HeightMap, height_map);
        zox_geter(tunk->value, VegetationMap, vegetation_map);
        if (!height_map->length || !biome_map->length || !vegetation_map->length) {
            zox_log_error("Invalid [Tunk] [Maps] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
        // ### ### ### ### ###
        // ### Get Biome Data ###*/
