zox_sys2(NoiseTextureSystem) {
    zox_change_check();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(Color);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureSize, textureSize)
        zox_sys_i(Color, fill_color)
        zox_sys_o(TextureData, textureData)
        zox_sys_o(GenerateTexture, generate)
        if (generate->value != zox_generate_texture_run) {
            continue;
        }
        byte texture_type = zox_texture_none;
        if (texture_mode == zox_texture_mode_graybox) {
            texture_type = zox_texture_graybox;
        } else if (zox_has(e, DirtTexture)) {
            texture_type = zox_texture_dirt;
        } else if (zox_has(e, GrassTexture)) {
            texture_type = zox_texture_grass;
        } else if (zox_has(e, SandTexture)) {
            texture_type = zox_texture_sand;
        } else if (zox_has(e, StoneTexture)) {
            texture_type = zox_texture_stone;
        } else if (zox_has(e, ObsidianTexture)) {
            texture_type = zox_texture_obsidian;
        }
        resize_TextureData(textureData, textureSize->value.x * textureSize->value.y);
        generate_texture_noise(textureData->value, textureSize->value, texture_type, terrain_texture_outline_type, fill_color->value);
        // generateTexture->value = 0;
        generate->value = zox_generate_texture_end;
        zox_add(e, TextureDirty);
    }
} zox_sys_end(NoiseTextureSystem);
