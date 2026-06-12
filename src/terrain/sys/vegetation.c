// NOTE: Places vegetation, only in top chunks
zox_sys2(VegetationChunk3System) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkPosition);
    zox_sys_in(TunkLink);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, state);
        zox_sys_i(NodeDepth, voctree_depth);
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_i(TunkLink, tunk);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(VoxelNodeDirty, dirty);
        if (state->value != zox_dirty_end) { // zox_dirty_active) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        zox_geter_value(terrain, RealmLink, entity, realm);
        if (!zox_valid(realm)) {
            continue;
        }
        zox_geter(realm, BiomeLinks, realm_biomes);
        if (!realm_biomes->length) {
            zox_log_error("No Biomes on Realm");
            continue;
        }
        zox_geter_value(terrain, NodeDepth, byte, terrain_depth);
        byte is_max_depth = voctree_depth->value == terrain_depth;
        byte voctree_length = powers_of_two_byte[voctree_depth->value];
        // int chunk_voxel_position_y = cposition->value.y * voctree_length;
        int terrain_chunk_length = powers_of_two[terrain_depth];
        int chunk_voxel_position_y = cposition->value.y *  terrain_chunk_length;
        int2 map_size = int2_single(terrain_chunk_length);
        byte3 positionl;
        int hmultiplier = 1;
        byte ccc = voctree_depth->value;
        while (ccc != terrain_depth) {
            hmultiplier *= 2;
            ccc++;
        }
        if (!zox_valid(tunk->value)) {
            zox_log_error("Invalid [Tunk] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
        zox_geter(tunk->value, HeightMap, height_map);
        zox_geter(tunk->value, BiomeMap, biome_map);
        zox_geter(tunk->value, VegetationMap, vegetation_map);
        if (!vegetation_map->length) {
            zox_log_error("Invalid [Tunk] [vegetation_map] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
        entity biome = 0;
        byte soil_grass_id = 0;
        byte grass_id = 0;
        byte wood_id = 0;
        write_lock_VoxelNode(voctree);
        for (positionl.x = 0; positionl.x < voctree_length; positionl.x++) {
            for (positionl.z = 0; positionl.z < voctree_length; positionl.z++) {
                int2 map_position = (int2) { positionl.x * hmultiplier, positionl.z * hmultiplier };
                int map_index = int2_array_index(map_position, map_size);
                byte biome_id = biome_map->value[map_index];
                byte veggie = vegetation_map->value[map_index];
                byte height = height_map->value[map_index];
                // Get Top Positions from Height Map
                int terrain_top_position = (int) height;
                int top_position = terrain_top_position - chunk_voxel_position_y;
                top_position /= hmultiplier;
                if (top_position < 0 || top_position >= voctree_length) {
                    continue;
                }
                // NOTE: Checks if outer bounds to determine if on top of world
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
                if (veggie >= 1) {
                    // NOTE: Make dirt Soil Grass
                    positionl.y = top_position;
                    set_VoxelNode(voctree, voctree_depth->value, positionl, soil_grass_id, 0);
                }
                // NOTE: Only do other vegetation if max depth
                if (!is_max_depth) {
                    continue;
                }
                if (veggie == 2) {
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
