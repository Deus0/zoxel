extern byte zox_maps_flip_x;
extern byte zox_maps_flip_z;

// NOTE: Simply creates a height texture from tunks
zox_sys2(HeightsTextureSystem) {
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
        // NOTE: Validate Tunks
        entity tunk = zox_get_link(world, e, Tunk);
#ifdef zox_safety_checks
        if (!zox_valid(tunk) ||
            !zox_has(tunk, GenerateTunk) ||
            !zox_has(tunk, HeightMap))
        {
            size->value = int2_single(0);
            resize_TextureData(data, size->value.x * size->value.y);
            zox_add(e, TextureDirty);
            continue;
        }
#endif
        entity terrain = zox_get_parent(world, tunk);
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_loge("Invalid [Terrain] for Texture [%s]", zox_get_name(e));
            continue;
        }
#endif
        // NOTE: Generation Delay for Tunks
        if (zox_getv(tunk, GenerateTunk)) {
            if (dbg_log) {
                zox_logw("Tunk Still Generating [%s]", zox_get_name(tunk));
            }
            continue;
        }
        byte lod = zox_getv(tunk, TunkLod);
        byte length = octree_size(lod);
        size->value = int2_single(length);
        const HeightMap* height_map = zox_get(tunk, HeightMap);
        if (length * length != height_map->length) {
            zox_loge("Invalid [Tunk] [Heightmap] for Texture [%s]", zox_get_name(e));
            continue;
        }
        if (dbg_log) {
            zox_log("Generating Tunk Texture [%ix%i]", size->value.x, size->value.y);
        }
        resize_TextureData(data, size->value.x * size->value.y);
        for (byte x = 0; x < length; x++) {
            for (byte y = 0; y < length; y++) {
                int index = int2_array_index((int2) { x, y }, size->value);
                byte value = height_map->value[index];
                value = int_clamp(32 + value * 10, 0, 255);
                index = int2_array_index((int2) { zox_maps_flip_x ? length - 1 - x : x, zox_maps_flip_z ? length - 1 - y : y }, size->value);
                data->value[index] = color_grayscale(value);
            }
        }
        generate->value = 0;
        zox_add(e, TextureDirty);
    }
} zox_sys_end(HeightsTextureSystem);
