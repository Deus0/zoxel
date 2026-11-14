extern byte zox_maps_flip_x;
extern byte zox_maps_flip_z;

// a map piece's texture: its tunk's top down colours (tunk_map_colors), flipped as maps are
void tunk_texture_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(TextureData);
    zox_sys_out(TextureSize);
    zox_sys_out(GenerateTexture);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureSize, size);
        zox_sys_o(GenerateTexture, generate);
        if (generate->value != zox_generate_texture_run) {
            continue;
        }
        const entity tunk = zox_get_link(world, e, TunkLink);
        if (!zox_valid(tunk) || !zox_has(tunk, HeightMap)) {
            size->value = int2_zero;
            resize_TextureData(data, 0);
            zox_add(e, TextureDirty);
            continue;
        }
        // waits while it or its chunks generate
        if (!tunk_map_colors(world, tunk)) {
            continue;
        }
        const TunkMapColors* colors = zox_get(tunk, TunkMapColors);
        const int side = (int) sqrtf((float) colors->length);
        size->value = int2_single(side);
        resize_TextureData(data, side * side);
        for (int x = 0; x < side; x++) {
            for (int z = 0; z < side; z++) {
                const int fx = zox_maps_flip_x ? side - 1 - x : x;
                const int fz = zox_maps_flip_z ? side - 1 - z : z;
                data->value[x + z * side] = colors->value[fx + fz * side];
            }
        }
        generate->value = 0;
        zox_add(e, TextureDirty);
    }
    zox_sys_on_end();
} zoxd_system(tunk_texture_system);
