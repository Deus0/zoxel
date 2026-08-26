zox_sys2(BricksModelGenerationSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_in(Color);
    zox_sys_in(VoxType);
    zox_sys_out(GenerateModel);
    zox_sys_out(VoxelNode);
    // zox_sys_out(VoxelNodeDirty);
    zox_sys_out(NodeDepth);
    zox_sys_out(ColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, seed);
        zox_sys_i(Color, fill);
        zox_sys_i(VoxType, gentype);
        zox_sys_o(GenerateModel, generate);
        zox_sys_o(VoxelNode, node);
        // zox_sys_o(VoxelNodeDirty, dirty);
        zox_sys_o(NodeDepth, depth);
        zox_sys_o(ColorRGBs, colors);
        if (generate->value != zox_generate_model_run) {
            continue;
        }
        if (gentype->value != vox_type_bricks) {
            continue;
        }
        if (!zox_has(e, SecondaryColor)) {
            zox_loge("[%s] has no SecondaryColor", zox_getn(e));
            continue;
        }
        byte unique_colors = zox_has(e, VoxUniqueColors) ? zox_getv(e, VoxUniqueColors) : default_unique_colors;
        float color_rr = zox_has(e, VoxColorRange) ? zox_getv(e, VoxColorRange) : default_color_range;
        float2 color_r = (float2) { 1 - color_rr, 1 + color_rr };
        byte node_depth = depth->value;
        // byte colors_count = unique_colors + zox_block_outlines;
        resize_ColorRGBs(colors, 0);
        color_rgb color_rgb_2 = color_to_color_rgb(fill->value);
        // colors to pick from
        byte2 vrange = (byte2) { 1, unique_colors - 1 };
        // generates random colors based on primary color
        srand(seed->value);
        for (int j = 0; j < unique_colors; j++) {
            color_rgb new_color = color_rgb_2;
            float m = randf_range(color_r.x, color_r.y);
            color_rgb_multiply_float(&new_color, m);
            add_to_ColorRGBs(colors, new_color);
        }
        color cracks_color = zox_getv(e, SecondaryColor);
        add_to_ColorRGBs(colors, color_to_color_rgb(cracks_color));
        byte crack_color = colors->length;
        // Write Locks node
        // write_lock_VoxelNode(node);
        build_vox_bricks(node, node_depth, vrange, crack_color);
        // Outlines
        if (zox_block_outlines) {
            add_to_ColorRGBs(colors, color_rgb_black);
            byte black_voxel = colors->length;
            vox_outlines(node, depth->value, black_voxel);
        }
        // write_unlock_VoxelNode(node);
        generate->value = zox_has(e, BakeModel) ? zox_generate_model_bake : zox_generate_model_end;
        // dirty->value = zox_dirty_trigger;
        zox_add(e, VoxelNodeDirty);
        if (dbg_log) {
            zox_log("Generated Vox [%s] Type [%i] Depth [%i]", zox_get_name(e), gentype->value, depth->value);
        }
    }
} zox_sys_end(BricksModelGenerationSystem);
