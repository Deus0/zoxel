// todo: generate colors in another system
// todo: split processes up to nodes
// todo: add unique colors as a property too
zox_sys2(VoxGenerationSystem) {
    byte dbg_log = 0;
    byte max_process = 1;   // No rush mate
    byte dbg_orientation = 0;
    byte dbg_whitebox = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_in(Color);
    zox_sys_in(VoxType);
    zox_sys_out(GenerateModel);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeDirty);
    zox_sys_out(NodeDepth);
    zox_sys_out(ColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, seed);
        zox_sys_i(Color, fill);
        zox_sys_i(VoxType, gentype);
        zox_sys_o(GenerateModel, generate);
        zox_sys_o(VoxelNode, node);
        zox_sys_o(VoxelNodeDirty, dirty);
        zox_sys_o(NodeDepth, depth);
        zox_sys_o(ColorRGBs, colors);
        if (generate->value != zox_generate_model_run) {
            continue;
        }
        if (max_process && process_count > max_process) {
            continue;
        }
        if (gentype->value != vox_type_blended && gentype->value != vox_type_rubble && gentype->value != vox_type_noisey && gentype->value != vox_type_bricks && gentype->value != vox_type_wood && gentype->value != vox_type_flowers) {
            continue;
        }
        srand(seed->value);
        byte unique_colors = zox_has(e, VoxUniqueColors) ? zox_getv(e, VoxUniqueColors) : default_unique_colors;
        if (dbg_orientation) {
            unique_colors = 16;
        }
        byte vregions = zox_has(e, VRegions) ? zox_getv(e, VRegions) :  16;
        float color_rr = zox_has(e, VoxColorRange) ? zox_getv(e, VoxColorRange) : default_color_range;
        float2 color_r = (float2) { 1 - color_rr, 1 + color_rr };
        byte node_depth = depth->value;
        // depth->value = node_depth;
        byte colors_count = unique_colors + is_generate_vox_outlines;
        resize_ColorRGBs(colors, colors_count);
        color_rgb color_rgb_2 = color_to_color_rgb(fill->value);
        // black color
        byte black_voxel = unique_colors + 1;
        // colors to pick from
        byte2 vrange = (byte2) { 1, unique_colors - 1 };
        // generates random colors based on primary color
        for (int j = 0; j < unique_colors; j++) {
            colors->value[j] = color_rgb_2;
            float m = randf_range(color_r.x, color_r.y);
            color_rgb_multiply_float(&colors->value[j], m);
        }
        if (dbg_orientation) {
            colors->value[0] = color_rgb_black;
            colors->value[1] = color_rgb_green;
            colors->value[2] = color_rgb_yellow;
            colors->value[3] = color_rgb_red;
            colors->value[4] = color_rgb_purple;
            colors->value[5] = color_rgb_blue;
            colors->value[6] = color_rgb_cyan;
        }
        if (is_generate_vox_outlines) {
            colors->value[unique_colors] = (color_rgb) { 0, 0, 0 };
        }
        if (dbg_whitebox) {
            colors->value[0] = color_rgb_white;
        }
        // Write Locks node
        write_lock_VoxelNode(node);
        // byte3 size = byte3_single(chunk_voxel_length);
        if (dbg_whitebox) {
            fill_octree(node, 1, node_depth);
        } else if (dbg_orientation) {
            build_vox_orientation_test(node, node_depth, 1, 2, 3, 4, 5, 6, 7);
        } else if (gentype->value == vox_type_blended) {
            zox_geter_value(e, SecondaryColor, color, under_color);
            for (int j = colors_count; j < colors_count + unique_colors; j++) {
                color_rgb new_color = color_to_color_rgb(under_color);
                float m = randf_range(color_r.x, color_r.y);
                color_rgb_multiply_float(&new_color, m);
                add_to_ColorRGBs(colors, new_color);
            }
            byte2 vrange_2 = (byte2) { colors_count + 1, colors_count + unique_colors };
            color_rgb dirt_dark_voxel = color_to_color_rgb(under_color);
            color_rgb_multiply_float(&dirt_dark_voxel, fracture_dark_multiplier);
            add_to_ColorRGBs(colors, dirt_dark_voxel);
            byte black_voxel_3 = colors->length;
            byte2 range_blend_1 = vrange;
            byte2 range_blend_2 = vrange_2;
            range_blend_2.y++; // include black
            color_rgb new_color = color_to_color_rgb(fill->value);
            color_rgb_multiply_float(&new_color, grass_blend_dark_multiplier);
            add_to_ColorRGBs(colors, new_color);
            byte black_voxel_2 = colors->length;
            // put indexes here
            build_vox_blended(node, node_depth, black_voxel_2, black_voxel_3, vrange, vrange_2, range_blend_1, range_blend_2, vregions);
        } else if (gentype->value == vox_type_noisey) {
            color_rgb dirt_dark_voxel = color_to_color_rgb(fill->value);
            color_rgb_multiply_float(&dirt_dark_voxel, fracture_dark_multiplier);
            add_to_ColorRGBs(colors, dirt_dark_voxel);
            byte black_voxel_3 = colors->length;
            vnoise3(node, node_depth, vrange, black_voxel_3);
            byte2 penetrations = (byte2) { 1, 5 };
            vnoise3_spray_side(node, node_depth, 0, penetrations, direction_up);
            vnoise3_spray_side(node, node_depth, 0, penetrations, direction_left);
            vnoise3_spray_side(node, node_depth, 0, penetrations, direction_right);
            vnoise3_spray_side(node, node_depth, 0, penetrations, direction_front);
            vnoise3_spray_side(node, node_depth, 0, penetrations, direction_back);
        } else if (gentype->value == vox_type_bricks) {
            color_rgb dirt_dark_voxel = color_to_color_rgb(fill->value);
            color_rgb_multiply_float(&dirt_dark_voxel, fracture_dark_multiplier);
            add_to_ColorRGBs(colors, dirt_dark_voxel);
            byte black_voxel_3 = colors->length;
            build_vox_bricks(node, node_depth, vrange, black_voxel_3);
        } else if (gentype->value == vox_type_wood) {
            byte2 wood = (byte2) { vrange.x, vrange.y / 2 };
            byte2 bark = (byte2) { wood.y + 1, vrange.y };
            for (byte k = wood.x; k < wood.y; k++) {
                color_rgb c = colors->value[k];
                float mul = k % 2 == 0 ? 0.9f : 1.3f;
                color_rgb_multiply_float(&c, mul);
                colors->value[k] = c;
            }
            for (byte k = bark.x; k < bark.y; k++) {
                color_rgb c = colors->value[k];
                color_rgb_multiply_float(&c, 0.6f);
                colors->value[k] = c;
            }
            build_vox_wood(node, node_depth, wood, bark);
        } else if (gentype->value == vox_type_flowers) {
            for (int j = vrange.x; j <= vrange.y / 2; j++) {
                float3 hsv = color_rgb_to_hsv(colors->value[j]);
                hsv = hsv_shift(
                    hsv,
                    rand_range(0, 100) >= 50 ? frand_range(160, 200) : frand_range(-200, -160),
                    frand_range(-16, 16),
                    frand_range(16, 32)
                );
                colors->value[j] = hsv_to_color_rgb(hsv);
                // colors->value[j] = color_rgb_flip(colors->value[j]);
            }
            color_rgb dirt_dark_voxel = color_to_color_rgb(fill->value);
            color_rgb_multiply_float(&dirt_dark_voxel, fracture_dark_multiplier);
            add_to_ColorRGBs(colors, dirt_dark_voxel);
            byte black_voxel_3 = colors->length;
            byte2 stem_range = (byte2) { vrange.x,
                vrange.x + (vrange.y - vrange.x) / 2
            };
            byte2 petal_range = (byte2) { stem_range.y, vrange.y };
            build_vox_flowers(node, node_depth, stem_range, petal_range, black_voxel_3);
        }  else if (gentype->value == vox_type_rubble) {
            byte max_height = zox_has(e, RubbleHeight) ? zox_getv(e, RubbleHeight) : 4;
            // build_vox_heights(node, node_depth, vrange, max_height);
            uint count = zox_has(e, RubbleCount) ? zox_getv(e, RubbleCount) : 200;
            build_vox_scatter(node, node_depth, vrange, max_height, count);
        } else {
            // zox_log_error("unknown vox type [%s]", zox_get_name(e));
            write_unlock_VoxelNode(node);
            continue;
        }
        if (is_generate_vox_outlines) {
            vox_outlines(node, node_depth, black_voxel);
        }
        write_unlock_VoxelNode(node);
        if (zox_has(e, BakeModel)) {
            generate->value = zox_generate_model_bake;
        } else {
            generate->value = zox_generate_model_end;
        }
        dirty->value = zox_dirty_trigger;
        if (zox_has(e, Busy)) {
            zox_set(e, Busy, { 0 });
        }
        if (dbg_log) {
            zox_log("Generated Vox [%s] Type [%i] Depth [%i]", zox_get_name(e), gentype->value, depth->value);
        }
        zox_sys_increment();
    }
} zox_sys_end(VoxGenerationSystem);
