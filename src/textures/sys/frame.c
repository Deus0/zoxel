// #define zox_time_frame_texture_system
zox_sys2(FrameTextureSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(FillColor);
    zox_sys_in(OutlineThickness);
    zox_sys_in(FrameCorner);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureSize, size);
        zox_sys_i(FillColor, fill);
        zox_sys_i(OutlineThickness, thickness);
        zox_sys_i(FrameCorner, edge);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureDirty, dirty);
        zox_sys_o(GenerateTexture, generate);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        resize_TextureData(data, size->value.x * size->value.y);
        byte add_noise = zox_has(e, TextureAddNoise);
        color fill_color = fill->value;
        color outline_color;
        if (zox_has(e, OutlineColor)) {
            outline_color = zox_getv(e, OutlineColor);
        } else {
            outline_color = fill_color;
        }
        generate_texture_frame(data->value, size->value, fill_color, outline_color, thickness->value, edge->value, add_noise);
        dirty->value = zox_dirty_trigger;
        //  voronoi2D(textureData->value, textureSize->value, color_gray, color_gray_dark, 0.7f);
        if (dbg_log) {
            zox_log("Frame Texture generated [%s] at [%f] fill [%ix%ix%ix%i]", zox_get_name(e), zox_current_time, fill->value.r, fill->value.g, fill->value.b, fill->value.a);
        }
    }
} zox_sys_end(FrameTextureSystem);
