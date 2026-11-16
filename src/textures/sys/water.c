// generates a water texture's frames (gen/water.c), its TextureData the first
void water_texture_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(Seed);
    zox_sys_in(FillColor);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    zox_sys_out(TextureFrames);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureSize, size);
        zox_sys_i(Seed, seed);
        zox_sys_i(FillColor, fill);
        zox_sys_o(GenerateTexture, generate);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureFrames, frames);
        if (generate->value != zox_generate_texture_run) {
            continue;
        }
        const int length = size->value.x * size->value.y;
        resize_TextureFrames(frames, length * zox_water_frames);
        generate_texture_water(frames->value, size->value, zox_water_frames, fill->value, (int) seed->value);
        resize_TextureData(data, length);
        memcpy(data->value, frames->value, length * sizeof(color));
        generate->value = zox_generate_texture_end;
        zox_add(e, TextureDirty);
    }
    zox_sys_on_end();
} zoxd_system(water_texture_system);
