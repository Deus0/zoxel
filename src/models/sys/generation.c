// todo: generate colors in another system
// todo: split processes up to nodes
// todo: add unique colors as a property too
zox_sys2(VoxGenerationSystem) {
    zox_ts_begin(vox_generation);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Color);
    zox_sys_in(VoxType);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeDirty);
    zox_sys_out(NodeDepth);
    zox_sys_out(ColorRGBs);
    byte any_dirty = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generateVox);

        if (generateVox->value == zox_dirty_active) {
            any_dirty = 1;
            break;
        }
    }
    if (!any_dirty) {
        zox_ts_end(vox_generation, 3, zox_profile_system_vox_generation);
        return;
    }
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Color, fill);
        zox_sys_i(VoxType, gentype);
        zox_sys_i(Generate, state);
        zox_sys_o(VoxelNode, node);
        zox_sys_o(VoxelNodeDirty, voxel_octree_dirty);
        zox_sys_o(NodeDepth, depth);
        zox_sys_o(ColorRGBs, colors);
        if (state->value != zox_dirty_active) {
            continue;
        }
        byte unique_colors = zox_has(e, VoxUniqueColors) ? zox_getv(e, VoxUniqueColors) : default_unique_colors;
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
        if (is_generate_vox_outlines) {
            colors->value[unique_colors] = (color_rgb) { 0, 0, 0 };
        }
        // Write Locks node
        write_lock_VoxelNode(node);
        // byte3 size = byte3_single(chunk_voxel_length);
        if (gentype->value == vox_type_soil) {
            // colors
            color_rgb dirt_dark_voxel = color_to_color_rgb(fill->value);
            color_rgb_multiply_float(&dirt_dark_voxel, fracture_dark_multiplier);
            add_to_ColorRGBs(colors, dirt_dark_voxel);
            byte black_voxel_3 = colors->length;
            build_vox_soil(node, node_depth, vrange, black_voxel_3, vregions);
        } else if (gentype->value == vox_type_blended) {
            // Colors
            zox_geter_value(e, SecondaryColor, color, under_color);
            // generates random colors based on secondary color
            for (int j = colors_count; j < colors_count + unique_colors; j++) {
                color_rgb new_color = color_to_color_rgb(under_color);
                float m = randf_range(color_r.x, color_r.y);
                color_rgb_multiply_float(&new_color, m);
                add_to_ColorRGBs(colors, new_color);
            }
            byte2 vrange_2 = (byte2) {
                colors_count + 1,
                colors_count + unique_colors

            };
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
        } else if (gentype->value == vox_type_rubble) {
            byte rubble_height = 4;
            if (zox_has(e, RubbleHeight)) {
                rubble_height = zox_get_value(e, RubbleHeight)
            }
            int rubble_count = 200;
            if (zox_has(e, RubbleCount)) {
                rubble_count = zox_get_value(e, RubbleCount)
            }
            build_vox_rubble(node, node_depth, vrange, rubble_count, rubble_height);
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
            continue;
            for (int j = vrange.x; j <= vrange.y / 2; j++) {
                colors->value[j] = color_rgb_flip(colors->value[j]);
            }
            color_rgb dirt_dark_voxel = color_to_color_rgb(fill->value);
            color_rgb_multiply_float(&dirt_dark_voxel, fracture_dark_multiplier);
            add_to_ColorRGBs(colors, dirt_dark_voxel);
            byte black_voxel_3 = colors->length;
            byte2 stem_range = (byte2) { vrange.x,
                vrange.x + (vrange.y - vrange.x) / 2
            };
            byte2 petal_range = (byte2) { stem_range.y, vrange.y };
            build_vox_flower_patch(node, node_depth, stem_range, petal_range, black_voxel_3);
        } else {
            zox_log_error("unknown vox type [%s]", zox_get_name(e));
            write_unlock_VoxelNode(node);
            continue;
        }
        if (is_generate_vox_outlines) {
            vox_outlines(node, node_depth, black_voxel);
        }
        // Unlocks the node
        write_unlock_VoxelNode(node);
        voxel_octree_dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(VoxGenerationSystem);
