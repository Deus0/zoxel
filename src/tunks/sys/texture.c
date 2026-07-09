// NOTE: Simply creates a height texture from tunks
zox_sys2(TunkTextureSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(TunkLink);
    zox_sys_out(TextureData);
    zox_sys_out(TextureSize);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(TunkLink, tunk);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureSize, size);
        zox_sys_o(TextureDirty, dirty);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        // NOTE: Validate Tunks
        if (!zox_valid(tunk->value) || !zox_has(tunk->value, GenerateTunk) || !zox_has(tunk->value, HeightMap)) {
            // zox_loge("Invalid [Tunk] for Texture [%s]", zox_get_name(e));
            size->value = int2_single(0);
            resize_TextureData(data, size->value.x * size->value.y);
            dirty->value = zox_dirty_trigger;
            continue;
        }
        entity terrain = zox_get_parent(world, tunk->value);
        // NOTE: Validate Terrain
        if (!zox_valid(terrain) || !zox_has(terrain, NodeDepth)) {
            zox_loge("Invalid [Terrain] for Texture [%s]", zox_get_name(e));
            continue;
        }
        // NOTE: Generation Delay for Tunks
        if (zox_getv(tunk->value, GenerateTunk)) {
            if (dbg_log) {
                zox_logw("Tunk Still Generating [%s]", zox_get_name(tunk->value));
            }
            zox_set(e, Generate, { zox_dirty_trigger });
            continue;
        }
        entity realm = zox_get_parent(world, terrain);
        const BlockLinks* blocks = zox_get(realm, BlockLinks);
        color block_colors[blocks->length];
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        const Chunk3Stack* stack = zox_get(tunk->value, Chunk3Stack);
        const VoxelNode* stackv[render_distance_y * 2 + 1];
        byte chunk_depth = terrain_depth;
        // NOTE: Fetch block colors when terrain changes
        for (byte k = 0; k < blocks->length; k++) {
            entity block = blocks->value[k];
            if (zox_valid(block)) {
                block_colors[k] = zox_getv(block, Color);
            } else {
                block_colors[k] = color_black;
            }
        }
        // NOTE: Fetch Chunk VoxelNode's
        for (sbyte k = render_distance_y * 2; k >= 0; k--) {
            entity chunk = stack->value[k];
            if (!zox_valid(chunk)) {
                stackv[k] = NULL;
            } else {
                stackv[k] = zox_get(chunk, VoxelNode);
                chunk_depth = zox_getv(chunk, NodeDepth);
            }
        }
        int map_length = powers_of_two[chunk_depth];
        size->value = int2_single(map_length);
        resize_TextureData(data, size->value.x * size->value.y);
        if (dbg_log) {
            zox_log("Generating Tunk Texture [%ix%i]", size->value.x, size->value.y);
        }
        byte3 position = byte3_zero;
        for (position.x = 0; position.x < map_length; position.x++) {
            for (position.z = 0; position.z < map_length; position.z++) {
                int index = int2_array_index((int2) { position.x, position.z }, size->value);
                // NOTE: From top of world, we cast down to find first block
                byte set_color = 0;
                for (sbyte k = render_distance_y * 2; k >= 0; k--) {
                    const VoxelNode* voxels = stackv[k];
                    if (!(voxels)) {
                        continue;
                    }
                    for (sbyte y = map_length - 1; y >= 0; y--) {
                        position.y = y;
                        byte voxel = getv_VoxelNode(voxels, position, chunk_depth);
                        if (!voxel) {
                            continue;
                        }
                        data->value[index] = block_colors[voxel - 1];
                        set_color = 1;
                        break;
                    }
                    if (set_color) {
                        break;
                    }
                }
            }
        }
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(TunkTextureSystem);
