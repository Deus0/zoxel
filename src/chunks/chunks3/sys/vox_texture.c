// using an axis and VoxLink, generates a texture by grabbing the first voxel on a given side
byte is_debug_sides = direction_none; // direction_none | direction_front
const color air_vox_color = { 15, 15, 25, 88 };

void generate_vox_debug_texture(
    color *data,
    const int2 size,
    byte side)
{
    color fill = air_vox_color;
    if (is_debug_sides == direction_up && side == direction_up) {
        fill = (color) { 255, 0, 0, 255 };
    } else if (is_debug_sides == direction_down && side == direction_down) {
        fill = (color) { 55, 0, 0, 255 };
    } else if (is_debug_sides == direction_back && side == direction_back) {
        fill = (color) { 0, 55, 0, 255 };
    } else if (is_debug_sides == direction_front && side == direction_front) {
        fill = (color) { 0, 255, 0, 255 };
    } else if (is_debug_sides == direction_left && side == direction_left) {
        fill = (color) { 0, 0, 255, 255 };
    } else if (is_debug_sides == direction_right && side == direction_right) {
        fill = (color) { 0, 0, 55, 255 };
    } else {
        return;
    }
    for (int i = 0; i < size.x * size.y; i++) {
        data[i] = fill;
    }
}

// TODO: Take in vox_size and texture_size
void generate_vox_texture(
    color* data,
    int2 size,
    const VoxelNode *chunk,
    const color_rgb *colors,
    byte side,
    byte depth,
    int2 toffset,
    int2 vox_texture_size,
    color air)
{
    if (!chunk) {
        return;
    }
    byte length = octree_size(depth);
    int2 tedge = vox_texture_size; // size;
    if (tedge.x > length || tedge.y > length) {
        zox_loge("[generate_vox_texture]: Edge OOB size: [%ix%i] depth: [%i] length: [%i]", tedge.x, tedge.y, depth, length);
        return;
    }
    int d = 0;
    if (side == direction_left || side == direction_right) {
        if (side == direction_right) {
            d = size.y - 1;
        }
        for (int i = 0; i < tedge.x; i++) {
            for (int j = 0; j < tedge.y; j++) {
                byte voxel = getv_VoxelNode(chunk, depth, (byte3) { d, j, i });
                byte is_darken = 0;
                if (!voxel) {
                    is_darken = 1;
                    if (side == direction_left) {
                        for (int x = 0; x < size.y; x++) {
                            voxel = getv_VoxelNode(chunk, depth, (byte3) { x, j, i });
                            if (voxel) {
                                break;
                            }
                        }
                    } else {
                        for (int x = size.y - 1; x >= 0; x--) {
                            voxel = getv_VoxelNode(chunk, depth, (byte3) { x, j, i });
                            if (voxel) {
                                break;
                            }
                        }
                    }
                }
                int2 pixel_position = int2_add((int2) { i, j }, toffset);
                int index = int2_array_index(pixel_position, size);
                if (voxel == 0) {
                    data[index] = air;
                } else {
                    data[index] = color_rgb_to_color(colors[voxel - 1]);
                }
                if (is_darken) {
                    data[index] = color_multiply_float(data[index], 0.8f);
                }
            }
        }
    } else if (side == direction_down || side == direction_up) {
        if (side == direction_up) {
            d = size.y - 1;
        }
        for (int i = 0; i < tedge.x; i++) {
            for (int j = 0; j < tedge.y; j++) {
                byte voxel = getv_VoxelNode(chunk, depth, (byte3) { i, d, j });
                byte is_darken = 0;
                if (!voxel) {
                    is_darken = 1;
                    if (side == direction_down) {
                        for (int k = 0; k < size.y; k++) {
                            voxel = getv_VoxelNode(chunk, depth, (byte3) { i, k, j });
                            if (voxel) {
                                break;
                            }
                        }
                    } else {
                        for (int k = size.y - 1; k >= 0; k--) {
                            voxel = getv_VoxelNode(chunk, depth, (byte3) { i, k, j });
                            if (voxel) {
                                break;
                            }
                        }
                    }
                }
                int2 pixel_position = int2_add((int2) { i, j }, toffset);
                int index = int2_array_index(pixel_position, size);
                if (voxel == 0) {
                    data[index] = air;
                } else {
                    data[index] = color_rgb_to_color(colors[voxel - 1]);
                }
                if (is_darken) {
                    data[index] = color_multiply_float(data[index], 0.8f);
                }
            }
        }
    } else if (side == direction_back || side == direction_front) {
        if (side == direction_front) {
            d = size.x - 1;
        }
        for (int i = 0; i < tedge.x; i++) {
            for (int j = 0; j < tedge.y; j++) {
                byte voxel = getv_VoxelNode(chunk, depth, (byte3) { i, j, d });
                byte is_darken = 0;
                if (!voxel) {
                    is_darken = 1;
                    if (side == direction_back) {
                        for (int k = 0; k < size.x; k++) {
                            voxel = getv_VoxelNode(chunk, depth, (byte3) { i, j, k });
                            if (voxel) {
                                break;
                            }
                        }
                    } else {
                        for (int k = size.x - 1; k >= 0; k--) {
                            voxel = getv_VoxelNode(chunk, depth, (byte3) { i, j, k });
                            if (voxel) {
                                break;
                            }
                        }
                    }
                }
                int2 pixel_position = int2_add((int2) { i, j }, toffset);
                int index = int2_array_index(pixel_position, size);
                if (voxel == 0) {
                    data[index] = air;
                } else {
                    data[index] = color_rgb_to_color(colors[voxel - 1]);
                }
                if (is_darken) {
                    data[index] = color_multiply_float(data[index], 0.8f);
                }
            }
        }

    }
}

// TODO: Offset by chunk size difference to node size
zox_sys2(VoxTextureSystem) {
    byte dbg_log = 0;
    // color air_color = (color) { 255, 0, 0, 255 };
    color debug_color = (color) { 15, 155, 15, 255 };
    color air_color = (color) { 0, 0, 0, 0 };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(VoxBakeSide);
    zox_sys_in(ModelLink);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureSize, size);
        zox_sys_i(VoxBakeSide, side);
        zox_sys_i(ModelLink, vox);
        zox_sys_o(GenerateTexture, generate);
        zox_sys_o(TextureData, data);
        // NOTE: It had to be delayed one frame due to the Generating Vox Time
        if (generate->value != zox_generate_texture_run) {
            continue;
        }
        if (!zox_valid(vox->value)) {
            zox_loge("Invalid Vox on Texture [%s]", zox_getn(e));
            continue;
        }
        if (!zox_has(vox->value, VoxelNode) ||
            !zox_has(vox->value, ColorRGBs) ||
            !zox_has(vox->value, ChunkSize) ||
            !zox_has(vox->value, NodeDepth))
        {
            zox_loge("Invalid Vox [%s] Components on Texture [%s]",
                zox_getn(vox->value),
                zox_getn(e));
            continue;
        }
        // NOTE: Delays the texture until its done
        if (zox_has(vox->value, GenerateModel) &&
            zox_getv(vox->value, GenerateModel))
        {
            if (dbg_log) {
                zox_log("Texture [%s] waiting on model to generate [%s]",
                    zox_get_name(e),
                    zox_get_name(vox->value));
            }
            continue;
        }
        // byte is_center = zox_has(e, CenterVoxTexture);
        zox_geter(vox->value, VoxelNode, voctree);
        zox_geter(vox->value, ColorRGBs, colors);
        int3 vox_size = zox_getv(vox->value, ChunkSize);
        // max depth tho
        byte bake_depth = next_power_of_two_root(int_max(size->value.x, size->value.y));
        byte vox_depth = zox_getv(vox->value, NodeDepth);
        // NOTE: We reduce bake depth if its greater than our models depth
        if (bake_depth > vox_depth) {
            bake_depth = vox_depth;
        }
        // we should offset texture and use a grab vox bounds
        int2 vox_texture_size = int2_zero;
        if (side->value == direction_front || side->value == direction_back) {
            vox_texture_size = (int2) { vox_size.x, vox_size.y };
        } else if (side->value == direction_left || side->value == direction_right) {
            vox_texture_size = (int2) { vox_size.y, vox_size.z };
        } else if (side->value == direction_down || side->value == direction_up) {
            vox_texture_size = (int2) { vox_size.x, vox_size.z };
        }
        // NOTE: Reduces by division when bake depth is less than model depth
        // TODO: Base this on depth difference instead
        for (byte j = bake_depth; j < vox_depth; j++) {
            vox_texture_size.x /= 2;
            vox_texture_size.y /= 2;
        }
        if (vox_texture_size.x > size->value.x ||
            vox_texture_size.y >  size->value.y)
        {
            zox_loge("Vox Texture e[%s] v[%s] Size (still) too large [%ix%i] > [%ix%i]",
                zox_get_name(e),
                zox_get_name(vox->value),
                vox_texture_size.x,
                vox_texture_size.y,
                size->value.x,
                size->value.y);
            continue;
        }
        int2 texture_offset = int2_sub(size->value, vox_texture_size);
        texture_offset = int2_divide_int(texture_offset, 2);
        int new_size = size->value.x * size->value.y;
        if (dbg_log >= 2) {
            zox_log("Vox Texture Debug e[%s] v[%s] offset %ix%i - vox_texture_size [%ix%i] - texture_size %ix%i - vox_size %ix%ix%i - pixels length [%i]",
                zox_get_name(e),
                zox_get_name(vox->value),
                texture_offset.x,
                texture_offset.y,
                vox_texture_size.x,
                vox_texture_size.y,
                size->value.x,
                size->value.y,
                vox_size.x,
                vox_size.y,
                vox_size.z,
                new_size);
        }
        if (new_size <= 0) {
            for (int j = 0; j < data->length; j++) {
                data->value[j] = debug_color;
            }
            zox_loge("No Size on Texture!!!");
            continue;
        }
        resize_TextureData(data, new_size);
        if (!data->value) {
            zox_loge("Failure resizing texture [%s]:[%ix%i]",
                zox_get_name(e),
                size->value.x,
                size->value.y);
            continue;
        }
        if (texture_offset.x || texture_offset.y) {
            // clear all to air first
            for (int j = 0; j < data->length; j++) {
                data->value[j] = air_color;
            }
        }
        // Testing
        // read_lock_VoxelNode(voctree);
        generate_vox_texture(
            data->value,
            size->value,
            voctree,
            colors->value,
            side->value,
            bake_depth,
            texture_offset,
            vox_texture_size,
            air_color);
        if (is_debug_sides != direction_none) {
            generate_vox_debug_texture(
                data->value,
                size->value,
                side->value);
        }
        // read_unlock_VoxelNode(voctree);
        if (texture_offset.x || texture_offset.y) {
            add_texture_outline(data->value, size->value, air_color, color_black);
        }
        generate->value = zox_generate_texture_end;
        zox_add(e, TextureDirty);
        if (dbg_log) {
            zox_log("Generated Vox Texture [%s]: Model [%s] Offset [%ix%i] vox_texture_size [%ix%i] texture_size [%ix%i] vox_size [%ix%ix%i] pixels length [%i]",
                zox_get_name(e),
                zox_get_name(vox->value),
                texture_offset.x,
                texture_offset.y,
                vox_texture_size.x,
                vox_texture_size.y,
                size->value.x,
                size->value.y,
                vox_size.x,
                vox_size.y,
                vox_size.z,
                new_size);
            zox_log("   - Bake Depth [%i] Side [%i] Vox Depth [%i] GenerateModel [%i]",
                bake_depth,
                side->value,
                vox_depth,
                zox_has(vox->value, GenerateModel));
        }
    }
} zox_sys_end(VoxTextureSystem);
