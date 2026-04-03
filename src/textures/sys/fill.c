zox_sys2(FillTextureSystem) {
    zox_change_check();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(Color);
    zox_sys_out(TextureData);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TextureSize, size);
        zox_sys_i(Color, c);
        zox_sys_o(TextureData, data);
        zox_sys_o(GenerateTexture, state);
        zox_sys_o(TextureDirty, dirty);

        if (state->value != zox_dirty_active) {
            continue;
        }

        resize_TextureData(data, size->value.x * size->value.y);

        generate_texture_fill(data->value, size->value, c->value);

        dirty->value = 1; // actually this only gets uploaded if has GPUTextureLink!
    }
} zox_sys_end(FillTextureSystem);
