zox_sys2(RubbleModelGenerationSystem) {
    byte dbg_log = 0;
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
        if (gentype->value != vox_type_rubble) {
            continue;
        }
        resize_ColorRGBs(colors, 0);
        byte unique_colors = zox_has(e, VoxUniqueColors) ? zox_getv(e, VoxUniqueColors) : default_unique_colors;
        float color_rr = zox_has(e, VoxColorRange) ? zox_getv(e, VoxColorRange) : default_color_range;
        float2 color_r = (float2) { 1 - color_rr, 1 + color_rr };
        byte node_depth = depth->value;
        color_rgb color_rgb_2 = color_to_color_rgb(fill->value);
        // colors to pick from
        byte2 vrange = (byte2) { 1, unique_colors - 1 };
        srand(seed->value);
        for (int j = 0; j < unique_colors; j++) {
            color_rgb new_color = color_rgb_2;
            float m = randf_range(color_r.x, color_r.y);
            color_rgb_multiply_float(&new_color, m);
            add_to_ColorRGBs(colors, new_color);
        }
        byte max_height = zox_has(e, RubbleHeight) ? zox_getv(e, RubbleHeight) : 4;
        // Write Locks node
        // write_lock_VoxelNode(node);
        // build_vox_heights(node, node_depth, vrange, max_height);
        uint count = zox_has(e, RubbleCount) ? zox_getv(e, RubbleCount) : 200;
        build_vox_scatter(node, node_depth, vrange, max_height, count);
        // Outlines
        if (is_generate_vox_outlines) {
            add_to_ColorRGBs(colors, color_rgb_black);
            byte black_voxel = colors->length;
            vox_outlines(node, depth->value, black_voxel);
        }
        // write_unlock_VoxelNode(node);
        generate->value = zox_has(e, BakeModel) ? zox_generate_model_bake : zox_generate_model_end;
        dirty->value = zox_dirty_trigger;
        if (dbg_log) {
            zox_log("Generated Vox [%s] Type [%i] Depth [%i]", zox_get_name(e), gentype->value, depth->value);
        }
    }
} zox_sys_end(RubbleModelGenerationSystem);
