extern byte zox_maps_flip_x;
extern byte zox_maps_flip_z;

// NOTE: Simply creates a height texture from tunks
zox_sys2(TunkTextureSystem) {
    byte dbg_log = 0;
    float color_boost = 0.5f;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    zox_sys_out(TextureSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(GenerateTexture, generate);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureSize, size);
        if (generate->value != zox_generate_texture_run) {
            continue;
        }
        // NOTE: Validate Tunks
        entity tunk = zox_get_link(world, e, Tunk);
        if (!zox_valid(tunk) ||
            !zox_has(tunk, GenerateTunk) ||
            !zox_has(tunk, HeightMap))
        {
            // zox_loge("Invalid [Tunk] for Texture [%s]", zox_get_name(e));
            size->value = int2_single(0);
            resize_TextureData(data, size->value.x * size->value.y);
            zox_add(e, TextureDirty);
            continue;
        }
        entity terrain = zox_get_parent(world, tunk);
        // NOTE: Validate Terrain
#ifdef zox_safety_checks
        if (!zox_valid(terrain) || !zox_has(terrain, NodeDepth)) {
            zox_loge("Invalid [Terrain] for Texture [%s]", zox_get_name(e));
            continue;
        }
#endif
        // NOTE: Generation Delay for Tunks
        if (zox_getv(tunk, GenerateTunk)) {
            if (dbg_log) {
                zox_logw("Tunk Still Generating [%s]", zox_get_name(tunk));
            }
            continue;
        }
        const Chunk3Stack* stack = zox_get(tunk, Chunk3Stack);
        byte chunks_busy = 0;
        for (int k = render_distance_y * 2; k >= 0; k--) {
            entity chunk = stack->value[k];
            if (!zox_valid(chunk)) {
                zox_loge("Chunk invalid in tunk stack [%s:%i]",
                    zox_get_name(tunk),
                    k);
                continue;
            }
            if (zox_has(chunk, GenerateChunk)) {
                chunks_busy = 1;
                break;
            }
        }
        if (chunks_busy) {
            continue;
        }
        entity realm = zox_get_parent(world, terrain);
        const BlockLinks* blocks = zox_get(realm, BlockLinks);
        color block_colors[blocks->length];
        byte terrain_depth = zox_getv(terrain, NodeDepth);
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
        for (int k = render_distance_y * 2; k >= 0; k--) {
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
        float height_div = (float) { render_distance_y * 2 * map_length };
        byte3 position = byte3_zero;
        for (position.x = 0; position.x < map_length; position.x++) {
            for (position.z = 0; position.z < map_length; position.z++) {
                int index = int2_array_index((int2) { position.x, position.z }, size->value);
                // NOTE: From top of world, we cast down to find first block
                byte lowest_voxel = 0;
                byte lowest_height = 255;
                for (int k = render_distance_y * 2; k >= 0; k--) {
                    const VoxelNode* voxels = stackv[k];
                    if (!(voxels)) {
                        continue;
                    }
                    for (sbyte y = map_length - 1; y >= 0; y--) {
                        position.y = y;
                        byte global_height = k * map_length + y;
                        byte voxel;
                        if (zox_maps_flip_x || zox_maps_flip_z) {
                            byte3 flipped_position = (byte3) {
                                zox_maps_flip_x ? map_length - 1 - position.x : position.x,
                                position.y,
                                zox_maps_flip_z ? map_length - 1 - position.z : position.z
                            };
                            voxel = getv_VoxelNode(voxels, chunk_depth, flipped_position);
                        } else {
                            voxel = getv_VoxelNode(voxels, chunk_depth, position);
                        }
                        if (!voxel) {
                            continue;
                        }
                        lowest_voxel = voxel;
                        lowest_height = global_height;
                        break;
                    }
                    if (lowest_voxel) {
                        break;
                    }
                }
                color block_color = lowest_voxel ? block_colors[lowest_voxel - 1] : color_black;
                float height_mul = (float) lowest_height / height_div;;
                data->value[index] = color_multiply_float(block_color, height_mul + color_boost);
            }
        }
        generate->value = 0;
        zox_add(e, TextureDirty);
    }
} zox_sys_end(TunkTextureSystem);
