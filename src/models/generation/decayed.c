void vnoise3_spray_side(
    VoxelNode* voctree,
    byte depth,
    byte value,
    byte2 penetrations,
    byte direction,
    uint* random_state)
{
    if (depth <= 1) {
        return;
    }

    byte length = powers_of_two_byte[depth];

    if (penetrations.x >= length) {
        penetrations.x = length - 1;
    }

    if (penetrations.y > length) {
        penetrations.y = length;
    }

    byte3 position;

    if (direction == direction_up) {
        for (position.x = 0; position.x < length; position.x++) {
            for (position.z = 0; position.z < length; position.z++) {
                byte penetration = noise_rand_range(
                    random_state,
                    penetrations.x,
                    penetrations.y);

                for (byte p = 0; p < penetration; p++) {
                    if (noise_rand(random_state) % 1000 > 600) {
                        break;
                    }

                    position.y = length - 1 - p;

                    set_VoxelNode(
                        voctree,
                        depth,
                        position,
                        value);
                }
            }
        }
        return;
    }

    if (direction == direction_right || direction == direction_left) {
        for (position.y = 0; position.y < length; position.y++) {
            for (position.z = 0; position.z < length; position.z++) {
                byte penetration = noise_rand_range(
                    random_state,
                    penetrations.x,
                    penetrations.y);

                for (byte p = 0; p < penetration; p++) {
                    if (noise_rand(random_state) % 1000 > 300) {
                        break;
                    }

                    position.x =
                        direction == direction_right ?
                            p :
                            length - 1 - p;

                    set_VoxelNode(
                        voctree,
                        depth,
                        position,
                        value);
                }
            }
        }
        return;
    }

    if (direction == direction_front || direction == direction_back) {
        for (position.y = 0; position.y < length; position.y++) {
            for (position.x = 0; position.x < length; position.x++) {
                byte penetration = noise_rand_range(
                    random_state,
                    penetrations.x,
                    penetrations.y);

                for (byte p = 0; p < penetration; p++) {
                    if (noise_rand(random_state) % 1000 > 300) {
                        break;
                    }

                    position.z =
                        direction == direction_front ?
                            p :
                            length - 1 - p;

                    set_VoxelNode(
                        voctree,
                        depth,
                        position,
                        value);
                }
            }
        }
        return;
    }

    zox_logw(
        "[vnoise3_spray_side] Unsupported direction [%i]",
        direction);
}

/*void vnoise3_spray_side(
    VoxelNode* voctree,
    byte depth,
    byte value,
    byte2 penetrations,
    byte direction)
{
    // NOTE: No need to spray at lower depths
    if (depth <= 1) {
        return;
    }
    byte length = powers_of_two_byte[depth];
    byte3 size = byte3_single(length);
    if (penetrations.x >= length) {
        penetrations.x = length - 1;
    }
    if (penetrations.y > length) {
        penetrations.y = length;
    }
    byte3 position;
    if (direction == direction_up) {
        for (position.x = 0; position.x < size.x; position.x++) {
            for (position.z = 0; position.z < size.z; position.z++) {
                byte penetration = rand_range(penetrations.x, penetrations.y);
                for (byte p = 0; p < penetration; p++) {
                    position.y = size.y - 1 - p;
                    int rando = rand() % 1000;
                    if (rando > 600) {
                        break;
                    }
                    set_VoxelNode(voctree, depth, position, value);
                }
            }
        }
    } else if (direction == direction_left || direction == direction_right) {
        for (position.y = 0; position.y < size.y; position.y++) {
            for (position.z = 0; position.z < size.z; position.z++) {
                byte penetration = rand_range(penetrations.x, penetrations.y);
                for (byte p = 0; p < penetration; p++) {
                    if (direction == direction_right) {
                        position.x = p;
                    } else {
                        position.x = size.x - 1 - p;
                    }
                    int rando = rand() % 1000;
                    if (rando > 300) {
                        break;
                    }
                    set_VoxelNode(voctree, depth, position, value);
                }
            }
        }
    } else if (direction == direction_back || direction == direction_front) {
        for (position.y = 0; position.y < size.y; position.y++) {
            for (position.x = 0; position.x < size.x; position.x++) {
                byte penetration = rand_range(penetrations.x, penetrations.y);
                for (byte p = 0; p < penetration; p++) {
                    if (direction == direction_front) {
                        position.z = p;
                    } else {
                        position.z = size.z - 1 - p;
                    }
                    int rando = rand() % 1000;
                    if (rando > 300) {
                        break;
                    }
                    set_VoxelNode(voctree, depth, position, value);
                }
            }
        }
    } else {
        zox_logw("[vnoise3_spray_side] Unsupported direction [%i]", direction);
    }
}*/

// todo: generate colors in another system
// todo: split processes up to nodes
// todo: add unique colors as a property too
void decayed_model_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_in(Color);
    zox_sys_in(VoxType);
    zox_sys_out(GenerateModel);
    zox_sys_out(VoxelNode);
    zox_sys_out(NodeDepth);
    zox_sys_out(ColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, seed);
        zox_sys_i(Color, fill);
        zox_sys_i(VoxType, gentype);
        zox_sys_o(GenerateModel, generate);
        zox_sys_o(VoxelNode, node);
        zox_sys_o(NodeDepth, depth);
        zox_sys_o(ColorRGBs, colors);
        if (generate->value != zox_generate_model_run) {
            continue;
        }
        if (gentype->value != vox_type_noisey) {
            continue;
        }
        uint random_state = seed->value ^ 0x9e3779b9u;
        resize_ColorRGBs(colors, 0);
        byte2 penetrations = (byte2) { 1, 5 };
        byte unique_colors =
            zox_has(e, VoxUniqueColors) ?
                zox_getv(e, VoxUniqueColors) :
                default_unique_colors;
        float color_rr =
            zox_has(e, VoxColorRange) ?
                zox_getv(e, VoxColorRange) :
                default_color_range;
        float2 color_r = (float2) {
            1 - color_rr,
            1 + color_rr
        };
        byte node_depth = depth->value;
        color_rgb color_rgb_2 = color_to_color_rgb(fill->value);
        byte2 vrange = (byte2) {
            1,
            unique_colors - 1
        };
        srand(seed->value);
        for (int j = 0; j < unique_colors; j++) {
            color_rgb new_color = color_rgb_2;
            float m = randf_range(color_r.x, color_r.y);
            color_rgb_multiply_float(&new_color, m);
            add_to_ColorRGBs(colors, new_color);
        }
        color_rgb dirt_dark_voxel = color_to_color_rgb(fill->value);
        color_rgb_multiply_float(&dirt_dark_voxel, fracture_dark_multiplier);
        add_to_ColorRGBs(colors, dirt_dark_voxel);
        byte black_voxel_3 = colors->length;
        vnoise3(
            node,
            node_depth,
            vrange,
            black_voxel_3,
            &random_state);
        // Write Locks node
        // write_lock_VoxelNode(node);
        for (int j = 0; j < 6; j++) {
            if (j == direction_down) {
                continue;
            }
            vnoise3_spray_side(
                node,
                node_depth,
                0,
                penetrations,
                j,
                &random_state);
        }
        if (zox_block_outlines) {
            add_to_ColorRGBs(colors, color_rgb_black);
            byte black_voxel = colors->length;
            vox_outlines(
                node,
                depth->value,
                black_voxel);
        }
        // write_unlock_VoxelNode(node);
        generate->value =
            zox_has(e, BakeModel) ?
                zox_generate_model_bake :
                zox_generate_model_end;
        zox_add(e, VoxelNodeDirty);
        if (dbg_log) {
            zox_log("Generated Vox [%s] Type [%i] Depth [%i]",
                zox_getn(e),
                gentype->value,
                depth->value);
        }
    }
    zox_sys_on_end();
} zoxd_system(decayed_model_system);
