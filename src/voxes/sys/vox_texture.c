// using an axis and VoxLink, generates a texture by grabbing the first voxel on a given side
byte is_debug_sides = block_side_none; // block_side_none | block_side_front
const color air_vox_color = { 15, 15, 25, 88 };

void generate_vox_debug_texture(color *data, const int2 size, byte side) {
    color fill = air_vox_color;
    if (is_debug_sides == block_side_up && side == block_side_up) {
        fill = (color) { 255, 0, 0, 255 };
    } else if (is_debug_sides == block_side_down && side == block_side_down) {
        fill = (color) { 55, 0, 0, 255 };
    } else if (is_debug_sides == block_side_back && side == block_side_back) {
        fill = (color) { 0, 55, 0, 255 };
    } else if (is_debug_sides == block_side_front && side == block_side_front) {
        fill = (color) { 0, 255, 0, 255 };
    } else if (is_debug_sides == block_side_left && side == block_side_left) {
        fill = (color) { 0, 0, 255, 255 };
    } else if (is_debug_sides == block_side_right && side == block_side_right) {
        fill = (color) { 0, 0, 55, 255 };
    } else {
        return;
    }
    for (int i = 0; i < size.x * size.y; i++) {
        data[i] = fill;
    }
}

// TODO: Take in vox_size and texture_size
void generate_vox_texture(color* data, int2 size, const VoxelNode *chunk, const color_rgb *colors, byte side, byte depth, int2 toffset, int2 vox_texture_size, color air_vox_color) {
    if (!chunk) {
        return;
    }
    byte vlength = powers_of_two[depth];
    int2 tedge = vox_texture_size; // size;
    if (tedge.x > vlength || tedge.y > vlength) {
        zox_loge("[generate_vox_texture]: Edge OOB size: [%ix%i] depth: [%i] length: [%i]", tedge.x, tedge.y, depth, vlength);
        return;
    }
    int d = 0;
    if (side == block_side_left || side == block_side_right) {
        if (side == block_side_right) {
            d = size.y - 1;
        }
        for (int i = 0; i < tedge.x; i++) {
            for (int j = 0; j < tedge.y; j++) {
                byte voxel = get_value_VoxelNode(chunk, depth, (byte3) { d, j, i }, 0);
                byte is_darken = 0;
                if (!voxel) {
                    is_darken = 1;
                    if (side == block_side_left) {
                        for (int x = 0; x < size.y; x++) {
                            voxel = get_value_VoxelNode(chunk, depth, (byte3) { x, j, i }, 0);
                            if (voxel) {
                                break;
                            }
                        }
                    } else {
                        for (int x = size.y - 1; x >= 0; x--) {
                            voxel = get_value_VoxelNode(chunk, depth, (byte3) { x, j, i }, 0);
                            if (voxel) {
                                break;
                            }
                        }
                    }
                }
                int2 pixel_position = int2_add((int2) { i, j }, toffset);
                int index = int2_array_index(pixel_position, size);
                if (voxel == 0) {
                    data[index] = air_vox_color;
                } else {
                    data[index] = color_rgb_to_color(colors[voxel - 1]);
                }
                if (is_darken) {
                    color_multiply_float(&data[index], 0.8f);
                }
            }
        }
    } else if (side == block_side_down || side == block_side_up) {
        if (side == block_side_up) {
            d = size.y - 1;
        }
        for (int i = 0; i < tedge.x; i++) {
            for (int j = 0; j < tedge.y; j++) {
                byte voxel = get_value_VoxelNode(chunk, depth, (byte3) { i, d, j }, 0);
                byte is_darken = 0;
                if (!voxel) {
                    is_darken = 1;
                    if (side == block_side_down) {
                        for (int k = 0; k < size.y; k++) {
                            voxel = get_value_VoxelNode(chunk, depth, (byte3) { i, k, j }, 0);
                            if (voxel) {
                                break;
                            }
                        }
                    } else {
                        for (int k = size.y - 1; k >= 0; k--) {
                            voxel = get_value_VoxelNode(chunk, depth, (byte3) { i, k, j }, 0);
                            if (voxel) {
                                break;
                            }
                        }
                    }
                }
                int2 pixel_position = int2_add((int2) { i, j }, toffset);
                int index = int2_array_index(pixel_position, size);
                if (voxel == 0) {
                    data[index] = air_vox_color;
                } else {
                    data[index] = color_rgb_to_color(colors[voxel - 1]);
                }
                if (is_darken) {
                    color_multiply_float(&data[index], 0.8f);
                }
            }
        }
    } else if (side == block_side_back || side == block_side_front) {
        if (side == block_side_front) {
            d = size.x - 1;
        }
        for (int i = 0; i < tedge.x; i++) {
            for (int j = 0; j < tedge.y; j++) {
                byte voxel = get_value_VoxelNode(chunk, depth, (byte3) { i, j, d }, 0);
                byte is_darken = 0;
                if (!voxel) {
                    is_darken = 1;
                    if (side == block_side_back) {
                        for (int k = 0; k < size.x; k++) {
                            voxel = get_value_VoxelNode(chunk, depth, (byte3) { i, j, k }, 0);
                            if (voxel) {
                                break;
                            }
                        }
                    } else {
                        for (int k = size.x - 1; k >= 0; k--) {
                            voxel = get_value_VoxelNode(chunk, depth, (byte3) { i, j, k }, 0);
                            if (voxel) {
                                break;
                            }
                        }
                    }
                }
                int2 pixel_position = int2_add((int2) { i, j }, toffset);
                int index = int2_array_index(pixel_position, size);
                if (voxel == 0) {
                    data[index] = air_vox_color;
                } else {
                    data[index] = color_rgb_to_color(colors[voxel - 1]);
                }
                if (is_darken) {
                    color_multiply_float(&data[index], 0.8f);
                }
            }
        }

    }
}

// TODO: Offset by chunk size difference to node size
zox_sys2(VoxTextureSystem) {
    byte dbg_log = 0;
    color air_color = (color) { 0, 0, 0, 0 };
    color debug_color = (color) { 15, 155, 15, 255 };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateTexture);
    zox_sys_in(TextureSize);
    zox_sys_in(VoxBakeSide);
    zox_sys_in(ModelLink);
    zox_sys_out(TextureData);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateTexture, state);
        zox_sys_i(TextureSize, size);
        zox_sys_i(VoxBakeSide, side);
        zox_sys_i(ModelLink, vox);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureDirty, dirty);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(vox->value) || !zox_has(vox->value, VoxelNode) || !zox_has(vox->value, ColorRGBs)  || !zox_has(vox->value, ChunkSize) || !zox_has(vox->value, NodeDepth)) {
            zox_loge("Invalid Vox on Texture [%s]", zox_get_name(e));
            continue;
        }
        // NOTE: Delays the texture until its done
        if (zox_has(vox->value, Generate) && zox_getv(vox->value, Generate)) {
            // zox_logw("Vox is Generating [%s]", zox_get_name(vox->value));
            zox_set(e, GenerateTexture, { zox_dirty_trigger });
            continue;
        }
        /*if (zox_has(vox->value, GenerateVox) && zox_getv(vox->value, GenerateVox)) {
            // zox_logw("Vox is Generating [%s]", zox_get_name(vox->value));
            zox_set(e, GenerateTexture, { zox_dirty_trigger });
            continue;
        }*/
        byte bake_depth = next_power_of_two_root(int_max(size->value.x, size->value.y));
        // byte is_center = zox_has(e, CenterVoxTexture);
        zox_geter(vox->value, VoxelNode, voctree);
        zox_geter(vox->value, ColorRGBs, colors);
        int3 vox_size = zox_getv(vox->value, ChunkSize);
        // max depth tho
        byte vox_depth = zox_getv(vox->value, NodeDepth);
        // NOTE: We reduce bake depth if its greater than our models depth
        if (bake_depth > vox_depth) {
            bake_depth = vox_depth;
        }
        // we should offset texture and use a grab vox bounds
        int2 vox_texture_size = int2_zero;
        if (side->value == block_side_front || side->value == block_side_back) {
            vox_texture_size = (int2) { vox_size.x, vox_size.y };
        } else if (side->value == block_side_left || side->value == block_side_right) {
            vox_texture_size = (int2) { vox_size.y, vox_size.z };
        } else if (side->value == block_side_down || side->value == block_side_up) {
            vox_texture_size = (int2) { vox_size.x, vox_size.z };
        }
        // NOTE: Reduces by division when bake depth is less than model depth
        // TODO: Base this on depth difference instead
        for (byte j = bake_depth; j < vox_depth; j++) {
            vox_texture_size.x /= 2;
            vox_texture_size.y /= 2;
        }
        if (vox_texture_size.x > size->value.x || vox_texture_size.y >  size->value.y) {
            zox_loge("Vox Texture e[%s] v[%s] Size (still) too large [%ix%i] > [%ix%i]", zox_get_name(e), zox_get_name(vox->value), vox_texture_size.x, vox_texture_size.y,  size->value.x,  size->value.y);
            continue;
        }
        /*while (vox_texture_size.x > size->value.x) {
            vox_texture_size.x /= 2;
        }
        while (vox_texture_size.y > size->value.y) {
            vox_texture_size.y /= 2;
        }*/
        int2 texture_offset = int2_sub(size->value, vox_texture_size);
        texture_offset = int2_divide_int(texture_offset, 2);
        int new_size = size->value.x * size->value.y;
        if (dbg_log >= 2) {
            zox_log("Vox Texture Debug e[%s] v[%s] offset %ix%i - vox_texture_size [%ix%i] - texture_size %ix%i - vox_size %ix%ix%i - pixels length [%i]", zox_get_name(e), zox_get_name(vox->value), texture_offset.x, texture_offset.y, vox_texture_size.x, vox_texture_size.y, size->value.x, size->value.y, vox_size.x, vox_size.y, vox_size.z, new_size);
        }
        if (new_size <= 0) {
            for (int j = 0; j < data->length; j++) {
                data->value[j] = debug_color;
            }
            continue;
        }
        resize_TextureData(data, new_size);
        if (!data->value) {
            zox_loge("Failure resizing texture [%s]:[%ix%i]", zox_get_name(e),  size->value.x, size->value.y);
            continue;
        }
        if (texture_offset.x || texture_offset.y) {
            // clear all to air first
            for (int j = 0; j < data->length; j++) {
                data->value[j] = air_color;
            }
        }
        // Testing
        read_lock_VoxelNode(voctree);
        generate_vox_texture(data->value, size->value, voctree, colors->value, side->value, bake_depth, texture_offset, vox_texture_size, air_color);
        if (is_debug_sides != block_side_none) {
            generate_vox_debug_texture(data->value, size->value, side->value);
        }
        read_unlock_VoxelNode(voctree);
        if (texture_offset.x || texture_offset.y) {
            add_texture_outline(data->value, size->value, air_color, color_black);
        }
        dirty->value = zox_dirty_trigger; // actually not using this for tilemap!
        if (dbg_log) {
            zox_log("Generated Vox Texture e[%s] v[%s] offset %ix%i - vox_texture_size [%ix%i] - texture_size %ix%i - vox_size %ix%ix%i - pixels length [%i]", zox_get_name(e), zox_get_name(vox->value), texture_offset.x, texture_offset.y, vox_texture_size.x, vox_texture_size.y, size->value.x, size->value.y, vox_size.x, vox_size.y, vox_size.z, new_size);
        }
    }
} zox_sys_end(VoxTextureSystem);
