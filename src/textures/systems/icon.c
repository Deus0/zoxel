void IconTextureSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(Color);
    zox_sys_in(OutlineColor);
    zox_sys_in(OutlineThickness);
    zox_sys_in(IconRadius);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    zox_sys_out(TextureDirty);

    for (int i = 0; i < it->count; i++) {

        zox_sys_e();
        zox_sys_i(TextureSize, size);
        zox_sys_i(Color, color_fill);
        zox_sys_i(OutlineColor, color_outline);
        zox_sys_i(OutlineThickness, outline_thickness);
        zox_sys_i(IconRadius, icon_radius);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureDirty, dirty);
        zox_sys_o(GenerateTexture, generate_texture);

        if (generate_texture->value != zox_generate_texture_generate) {
            continue;
        }

        initialize_TextureData(data, size->value.x * size->value.y);

        const byte add_noise = zox_has(e, TextureAddNoise);
        const byte radius = (byte) (size->value.y * icon_radius->value);

        generate_texture_icon(
            data->value,
            size->value,
            color_fill->value,
            color_outline->value,
            outline_thickness->value,
            radius,
            add_noise
        );
        dirty->value = 1;

        // zox_log(" > [%s] radius [%i] size [%ix%i]\n", zox_get_name(it->entities[i]), radius, textureSize->value.x, textureSize->value.y)

    }
} zoxd_system2(IconTextureSystem);