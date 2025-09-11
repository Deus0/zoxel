// #define zox_time_frame_texture_system
void FrameTextureSystem(iter *it) {
    zox_change_check();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(Color);
    zox_sys_in(OutlineThickness);
    zox_sys_in(FrameCorner);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureSize, size);
        zox_sys_i(Color, color2);
        zox_sys_i(OutlineThickness, thickness);
        zox_sys_i(FrameCorner, edge);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureDirty, dirty);
        zox_sys_o(GenerateTexture, generate);

        if (generate->value != zox_dirty_active) {
            continue;
        }

        initialize_TextureData(data, size->value.x * size->value.y);
        const byte add_noise = zox_has(e, TextureAddNoise);
        const color fill_color = color2->value;
        color outline_color;
        if (zox_has(e, OutlineColor)) {
            outline_color = zox_get_value(e, OutlineColor)
        } else {
            outline_color = (color) {
                fill_color.g + 25 + rand() % 25,
                fill_color.b + 25 + rand() % 25,
                fill_color.r + 25 + rand() % 25,
                255
            };
        }

        generate_texture_frame(
            data->value,
            size->value,
            fill_color,
            outline_color,
            thickness->value,
            edge->value,
            add_noise
        );

        // zox_log("+ texture generated [%s]", zox_get_name(e))
        dirty->value = 1;
        //  voronoi2D(textureData->value, textureSize->value, color_gray, color_gray_dark, 0.7f);
    }
} zoxd_system2(FrameTextureSystem);