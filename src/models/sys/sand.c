void build_vox_sand(ColorRGBs *colors, VoxelNode *vox, byte depth, color_rgb primary) {
    byte unique_colors = 10;
    byte first = colors->length + 1;
    // Mystical palette (dark -> glowing)
    for (byte i = 0; i < unique_colors; i++) {
        float t = (float)i / (unique_colors - 1);
        color_rgb c = primary;
        // Dark -> bright
        float brightness = 0.55f + 0.75f * t;
        // Small random variation
        brightness += frand_range(-0.04f, 0.04f);
        color_rgb_multiply_float(&c, brightness);
        // Slight warm/cool shift
        if (i & 1) c.r = byte_min(255, c.r + 6);
        else c.b = byte_min(255, c.b + 6);
        add_to_ColorRGBs(colors, c);
    }
    /*for (byte i = 0; i < unique_colors; i++) {
        float t = (float)i / (unique_colors - 1);
        color_rgb c = primary;
        color_rgb_multiply_float(&c, 0.55f + 0.8f * t);
        // Cool shadows
        c.b = byte_min(255, c.b + (byte)((1.0f - t) * 18));
        // Warm highlights
        c.r = byte_min(255, c.r + (byte)(t * 12));
        c.g = byte_min(255, c.g + (byte)(t * 8));
        add_to_ColorRGBs(colors, c);
    }*/
    byte size = powers_of_two_byte[depth];
    byte3 pos;
    const float TAU = 6.28318530718f;
    for (pos.x = 0; pos.x < size; pos.x++)
    for (pos.y = 0; pos.y < size; pos.y++)
    for (pos.z = 0; pos.z < size; pos.z++) {
        float x = TAU * (float) pos.x / size;
        float y = TAU * (float) pos.y / size;
        float z = TAU * (float) pos.z / size;
        // Three intertwined magical fields.
        float f =
            sinf(x*3.0f + cosf(z*2.0f)) +
            sinf(y*4.0f + cosf(x*3.0f)) +
            sinf(z*5.0f + cosf(y*2.0f));
        // Swirling interference.
        f +=    0.7f * sinf(x+y+z) +
                0.5f * cosf(2*x-y+z) +
                0.4f * sinf(3*z-x);
        // Normalize.
        f = (f + 4.6f) / 9.2f;
        if (f < 0.0f) f = 0.0f;
        if (f > 1.0f) f = 1.0f;
        byte color = first +
            (byte)(f * (unique_colors - 1));
        set_VoxelNode(vox, depth, pos, color);
    }
}

// NOTE: Generates a Sand block
zox_sys2(SandModelGenerationSystem) {
    byte dbg_log = 0;
    byte max_process = 2;   // TODO: Make work without breaking
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Color);
    zox_sys_in(VoxType);
    zox_sys_out(GenerateModel);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeDirty);
    zox_sys_out(NodeDepth);
    zox_sys_out(ColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
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
        if (gentype->value != vox_type_sand) {
            continue;
        }
        // Reset Data
        resize_ColorRGBs(colors, 0);
        // Generate Vox
        write_lock_VoxelNode(node);
        // Build Road Vox
        color_rgb primary_rgb = color_to_color_rgb(fill->value);
        build_vox_sand(colors, node, depth->value, primary_rgb);
        // Outlines
        if (is_generate_vox_outlines) {
            byte black_voxel = colors->length + 1;
            add_to_ColorRGBs(colors, color_rgb_black);
            vox_outlines(node, depth->value, black_voxel);
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
            zox_log("Generated Sand Vox [%s]:%i", zox_get_name(e), gentype->value);
        }
        zox_sys_increment();
    }
} zox_sys_end(SandModelGenerationSystem);