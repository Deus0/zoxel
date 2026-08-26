// todo: generate colors in another system
// todo: split processes up to nodes
// todo: add unique colors as a property too
zox_sys2(SoilGenerationSystem) {
    byte dbg_log = 0;
    // TODO: Make work without breaking
    byte max_process = !zox_disable_process_skips ? 1 : 0;
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
        if (max_process && process_count > max_process) {
            continue;
        }
        if (gentype->value != vox_type_soil) {
            continue;
        }
        byte unique_colors = zox_has(e, VoxUniqueColors) ? zox_getv(e, VoxUniqueColors) : default_unique_colors;
        // Generate Colors
        float color_rr = zox_has(e, VoxColorRange) ? zox_getv(e, VoxColorRange) : default_color_range;
        float2 color_r = (float2) { 1 - color_rr, 1 + color_rr };
        byte colors_count = unique_colors + zox_block_outlines;
        resize_ColorRGBs(colors, colors_count);
        color_rgb color_rgb_2 = color_to_color_rgb(fill->value);
        byte2 vrange = (byte2) { 1, unique_colors - 1 };
        srand(seed->value);
        for (int j = 0; j < unique_colors; j++) {
            colors->value[j] = color_rgb_2;
            float m = randf_range(color_r.x, color_r.y);
            color_rgb_multiply_float(&colors->value[j], m);
        }
        color_rgb dirt_dark_voxel = color_to_color_rgb(fill->value);
        color_rgb_multiply_float(&dirt_dark_voxel, fracture_dark_multiplier);
        add_to_ColorRGBs(colors, dirt_dark_voxel);
        byte black_voxel_3 = colors->length;
        // Generate Vox
        byte vregions = zox_has(e, VRegions) ? zox_getv(e, VRegions) :  16;
        // write_lock_VoxelNode(node);
        build_vox_soil(node, depth->value, vrange, black_voxel_3, vregions);
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
            zox_log("Generated Vox [%s]:%i", zox_get_name(e), gentype->value);
        }
        zox_sys_increment();
    }
} zox_sys_end(SoilGenerationSystem);
