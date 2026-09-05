// generates our terrain voxels
zox_sys2(FlatlandSystem) {
    byte target_depth = terrain_depth;
    byte chunk_voxel_length = powers_of_two_byte[target_depth];
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(ChunkPosition);
    zox_sys_out(VoxelNode);
    zox_sys_out(NodeDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(ChunkPosition, positionc);
        zox_sys_o(VoxelNode, node);
        zox_sys_o(NodeDepth, node_depth);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        node_depth->value = target_depth;
        float3 chunk_positionc_float3 = float3_from_int3(positionc->value);
        int chunk_positionc_y = (int) (chunk_positionc_float3.y * chunk_voxel_length);
        if (chunk_positionc_y >= 0) {
            fill_octree(node, 0, 0);
            continue;
        }
        zox_geter(terrain->value, RealmLink, realm);
        zox_geter(realm->value, BiomeLinks, biomes);
        if (!biomes->length) {
            zox_log_error("No Biomes");
            continue;
        }
        entity biome = positionc->value.z > 0 ? biomes->value[0] :  biomes->value[biomes->length - 1];
        zox_geter(biome, BlockLinks, biome_blocks);
        if (!biome_blocks->length) {
            zox_log_error("No Blocks in Biome [%s]", zox_get_name(biome));
            continue;
        }
        const entity dirt = biome_blocks->value[0];
        if (!zox_valid(dirt)) {
            zox_log_error("Dirt [%s] invalid in Biome [%s]", zox_get_name(biome));
            continue;
        }
        zox_geter_value(dirt, BlockIndex, byte, biome_dirt_id);
        if (!biome_dirt_id) {
            zox_log_error("Biome dirt is air.");
            continue;
        }
        fill_octree(node, biome_dirt_id, 0);
    }
} zox_sys_end(FlatlandSystem);
