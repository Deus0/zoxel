// using an axis and VoxLink, generates a texture by grabbing the first voxel on a given side
byte is_debug_sides = block_side_none; // block_side_none | block_side_front
const color air_vox_color = { 15, 15, 25, 88 };

void generate_vox_debug_texture(color *data, const int2 size, byte side) {
    if (is_debug_sides == block_side_none) {
        return;
    }
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

void generate_vox_texture(color *data, int2 size, const VoxelNode *chunk, const color_rgb *colors, byte side, byte max_depth, int2 toffset, int2 tclip) {
    if (!chunk) {
        return;
    }
    int2 tedge = size;
    // if using a cutout of the texture
    if (tclip.x) tedge.x = size.x < tclip.x ? size.x : tclip.x;
    if (tclip.y) tedge.y = size.y < tclip.y ? size.y : tclip.y;
    int index = 0;
    int d = 0;
    if (side == block_side_left || side == block_side_right) {
        if (side == block_side_right) {
            d = size.y - 1;
        }
        for (int z = toffset.x; z < tedge.x; z++) {
            for (int y = toffset.y; y < tedge.y; y++) {
                // byte3 node_position = (byte3) { d, y, z };
                // byte voxel = get_sub_node_voxel(chunk, &node_position, max_depth);
                byte voxel = get_value_VoxelNode(chunk, max_depth, (byte3) { d, y, z }, 0);
                byte is_darken = 0;
                if (!voxel) {
                    is_darken = 1;
                    if (side == block_side_left) {
                        for (int x = 0; x < size.y; x++) {
                            // node_position = (byte3) { x, y, z };
                            // voxel = get_sub_node_voxel(chunk, &node_position, max_depth);
                            voxel = get_value_VoxelNode(chunk, max_depth, (byte3) { x, y, z }, 0);
                            if (voxel) {
                                break;
                            }
                        }
                    } else {
                        for (int x = size.y - 1; x >= 0; x--) {
                            // node_position = (byte3) { x, y, z };
                            // voxel = get_sub_node_voxel(chunk, &node_position, max_depth);
                            voxel = get_value_VoxelNode(chunk, max_depth, (byte3) { x, y, z }, 0);
                            if (voxel) {
                                break;
                            }
                        }
                    }
                }
                index = int2_array_index((int2) { z, y }, size );
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
        for (int i = toffset.x; i < tedge.x; i++) {
            for (int j = toffset.y; j < tedge.y; j++) {
                //byte3 node_position = (byte3) { i, d, j };
                //byte voxel = get_sub_node_voxel(chunk, &node_position, max_depth);
                byte voxel = get_value_VoxelNode(chunk, max_depth, (byte3) { i, d, j }, 0);
                byte is_darken = 0;
                if (voxel == 0) {
                    is_darken = 1;
                    if (side == block_side_down) {
                        for (int k = 0; k < size.y; k++) {
                            // node_position = (byte3) { i, k, j };
                            // voxel = get_sub_node_voxel(chunk, &node_position, max_depth);
                            voxel = get_value_VoxelNode(chunk, max_depth, (byte3) { i, k, j }, 0);
                            if (voxel) {
                                break;
                            }
                        }
                    } else {
                        for (int k = size.y - 1; k >= 0; k--) {
                            //node_position = (byte3) { i, k, j };
                            //voxel = get_sub_node_voxel(chunk, &node_position, max_depth);
                            voxel = get_value_VoxelNode(chunk, max_depth, (byte3) { i, k, j }, 0);
                            if (voxel) {
                                break;
                            }
                        }
                    }
                }
                index = int2_array_index((int2) { i, j }, size);
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
        for (int i = toffset.x; i < tedge.x; i++) {
            for (int j = toffset.y; j < tedge.y; j++) {
                //byte3 node_position = (byte3) { i, j, d };
                //byte voxel = get_sub_node_voxel(chunk, &node_position, max_depth);
                byte voxel = get_value_VoxelNode(chunk, max_depth, (byte3) { i, j, d }, 0);
                byte is_darken = 0;
                if (voxel == 0) {
                    is_darken = 1;
                    if (side == block_side_back) {
                        for (int k = 0; k < size.x; k++) {
                            // node_position = (byte3) { i, j, k };
                            // voxel = get_sub_node_voxel(chunk, &node_position, max_depth);
                            voxel = get_value_VoxelNode(chunk, max_depth, (byte3) { i, j, k }, 0);
                            if (voxel) break;
                        }
                    } else {
                        for (int k = size.x - 1; k >= 0; k--) {
                            //node_position = (byte3) { i, j, k };
                            //voxel = get_sub_node_voxel(chunk, &node_position, max_depth);
                            voxel = get_value_VoxelNode(chunk, max_depth, (byte3) { i, j, k }, 0);
                            if (voxel) {
                                break;
                            }
                        }
                    }
                }
                index = int2_array_index((int2) { i, j }, size);
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
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(VoxLink);
    zox_sys_in(VoxBakeSide);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxLink, vox);
        zox_sys_i(TextureSize, size);
        zox_sys_i(VoxBakeSide, side);
        zox_sys_o(GenerateTexture, generate);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureDirty, dirty);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(vox->value) || !zox_has(vox->value, VoxelNode) ||
            (zox_has(vox->value, Generate) && zox_gett_value(vox->value, Generate))) {

            zox_logw("Vox Cannot Generate [%s]", zox_get_name(vox->value));
            continue;
        }
        // TODO: Finish Center Feature
        byte center = 0; // zox_has(e, CenterVoxTexture);
        zox_geter(vox->value, ColorRGBs, colors);
        zox_geter(vox->value, VoxelNode, voctree);
        zox_geter_value(vox->value, ChunkSize, int3, csize);
        zox_geter_value(vox->value, NodeDepth, byte, ndepth);
        int2 tsize = size->value;
        // we should offset texture and use a grab vox bounds
        int2 toffset = center ? (int2) { tsize.x - csize.x, tsize.y - csize.y } : int2_zero;
        // TODO: use csize rotated for the direction
        int2 tclip = center ? (int2) { csize.x, csize.y } : int2_zero;
        // int2 toffset = center ? (int2) { 4, 4 } : int2_zero;
        // int2 tclip = center ? (int2) { 8, 8 } : int2_zero;
        if (center) {
            zox_log("+ e[%s] v[%s] toffsetting %ix%i - tclip [%ix%i] - tsize %ix%i - csize %ix%i", zox_get_name(e), zox_get_name(vox->value), toffset.x, toffset.y, tclip.x, tclip.y, tsize.x, tsize.y, csize.x, csize.y);
        }
        resize_TextureData(data, tsize.x * tsize.y);
        read_lock_VoxelNode(voctree);
        generate_vox_texture(data->value, tsize, voctree, colors->value, side->value, ndepth, toffset, tclip);
        generate_vox_debug_texture(data->value, tsize, side->value);
        read_unlock_VoxelNode(voctree);
        dirty->value = zox_dirty_trigger; // actually not using this for tilemap!
        // zox_log("Generated Vox Texture [%s]: %ix%i: %ix%ix%i", zox_get_name(vox->value), texture_size.x, texture_size.y, csize.x, csize.y, csize.z);
    }
} zox_sys_end(VoxTextureSystem);
