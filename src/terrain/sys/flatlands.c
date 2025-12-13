// generates our terrain voxels
void FlatlandSystem(iter *it) {
    const byte target_depth = terrain_depth;
    const byte chunk_voxel_length = powers_of_two_byte[target_depth];
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateChunk);
    zox_sys_in(ChunkPosition);
    zox_sys_in(VoxLink);
    zox_sys_out(VoxelNode);
    zox_sys_out(NodeDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateChunk, generate);
        zox_sys_i(ChunkPosition, positionc);
        zox_sys_i(VoxLink, terrain);
        zox_sys_o(VoxelNode, node);
        zox_sys_o(NodeDepth, node_depth);

        if (generate->value != chunk_generate_state_update) {
            continue;
        }

        node_depth->value = target_depth;

        const float3 chunk_positionc_float3 = float3_from_int3(positionc->value);
        const int chunk_positionc_y = (int) (chunk_positionc_float3.y * chunk_voxel_length);

        if (chunk_positionc_y >= 0) {
            fill_new_octree(node, 0, 0);
            continue;
        }

        zox_geter(terrain->value, RealmLink, realm);
        zox_geter(realm->value, BiomeLinks, biomes);
        if (!biomes->length) {
            zox_log_error("No Biomes");
            continue;
        }
        const entity biome = positionc->value.z > 0 ? biomes->value[0] :  biomes->value[biomes->length - 1];
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

        fill_new_octree(node, biome_dirt_id, 0);
    }
} zoxd_system2(FlatlandSystem);
