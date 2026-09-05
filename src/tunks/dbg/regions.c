// NOTE: Simply creates a height texture from tunks
zox_sys2(RegionTextureSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    zox_sys_out(TextureSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(GenerateTexture, generate);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureSize, size);
        if (generate->value != zox_generate_texture_run) {
            continue;
        }
        entity tunk = zox_get_link(world, e, Tunk);
#ifdef zox_safety_checks
        if (!zox_valid(tunk) ||
            !zox_has(tunk, GenerateTunk))
        {
            zox_loge("Invalid Tunk in Region Maps");
            size->value = int2_single(0);
            resize_TextureData(data, size->value.x * size->value.y);
            zox_add(e, TextureDirty);
            continue;
        }
#endif
        entity region = zox_get_link(world, tunk, Region);
#ifdef zox_safety_checks
        if (!zox_valid(region) || !zox_has(region, Seed)) {
            zox_loge("Invalid [Region] for Texture [%s]", zox_get_name(e));
            continue;
        }
#endif
        byte lod = zox_getv(tunk, TunkLod);
        byte length = octree_size(lod);
        size->value = int2_single(length);
        lint region_seed = zox_getv(region, Seed);
        color region_color = color_grayscale(seed_range(region_seed, 0, 255));
        resize_TextureData(data, size->value.x * size->value.y);
        if (dbg_log) {
            zox_log("Generating Regions Texture [%ix%i] Seed [%i]", size->value.x, size->value.y, region_seed);
        }
        for (byte x = 0; x < length; x++) {
            for (byte y = 0; y < length; y++) {
                int index = int2_array_index((int2) { x, y }, size->value);
                data->value[index] = region_color;
            }
        }
        generate->value = 0;
        zox_add(e, TextureDirty);
    }
} zox_sys_end(RegionTextureSystem);
