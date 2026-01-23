

byte cache_blocks_data(iter* it, chunk3_textured_builder_data* data) {
    // chunk3_textured_builder_data build_data;

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockManagerLink);

    entity manager = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(BlockManagerLink, blocker);

        if (!zox_valid(blocker->value)) {
            continue;
        }

        manager = blocker->value;

        break;
    }

    if (!manager) {
        return 0;
    }

    zox_geter(manager, BlockLinks, blocks);
    if (!blocks->length) {
        return 0; // if failed to find terrain parents
    }

    // byte solidity[blocks->length];
    // build_data.solidity = solidity;
    // int uvs[blocks->length * 6];
    // build_data.uvs = uvs;
    // data->solidity = byte[blocks->length];
    // data->uvs = int[blocks->length * 6];
    data->solidity = malloc(blocks->length * sizeof(byte));
    data->uvs      = malloc(blocks->length * 6 * sizeof(int));

    int uvs_index = 0;

    for (int i = 0; i < blocks->length; i++) {
        entity block = blocks->value[i];
        if (!zox_valid(block)) {
            data->solidity[i] = 1;
            continue;
        }

        // solidity
        if (!zox_has(block, BlockModel)) {
            data->solidity[i] = 1;
        } else {
            data->solidity[i] = zox_gett_value(block, BlockModel) == zox_block_solid;
        }

        // Cache the UVs if exist
        if (!zox_has(block, TextureLinks)) {
            continue;
        }

        zox_geter(block, TextureLinks, block_textures);
        byte block_textures_length = block_textures->length;
        int voxel_uv_indexes_index = i * 6;
        if (block_textures_length == 1) {
            // per voxel, 24 uvs
            for (byte k = 0; k < 6; k++) {
                data->uvs[voxel_uv_indexes_index + k] = uvs_index;
                // uvs_index += 4;
            }
            uvs_index += 4;
        } else {
            // for 6 sides textured voxes
            for (byte k = 0; k < 6; k++) {
                data->uvs[voxel_uv_indexes_index + k] = uvs_index;
                uvs_index += 4;
            }
        }
    }

    return 1;
}