// NOTE: Simply creates a height texture from tunks
zox_sys2(TownTextureSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TunkLink);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    zox_sys_out(TextureSize);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TunkLink, tunk);
        zox_sys_o(GenerateTexture, generate);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureSize, size);
        zox_sys_o(TextureDirty, dirty);
        if (generate->value != zox_generate_texture_run) {
            continue;
        }
#ifdef zox_safety_checks
        if (!zox_valid(tunk->value) || !zox_has(tunk->value, GenerateTunk) || !zox_has(tunk->value, HeightMap)) {
            zox_loge("Invalid [Tunk] for Texture [%s]", zox_get_name(e));
            size->value = int2_single(0);
            resize_TextureData(data, size->value.x * size->value.y);
            dirty->value = zox_dirty_trigger;
            continue;
        }
#endif
        entity terrain = zox_get_parent(world, tunk->value);
#ifdef zox_safety_checks
        if (!zox_valid(terrain) || !zox_has(terrain, NodeDepth)) {
            zox_loge("Invalid [Terrain] for Texture [%s]", zox_get_name(e));
            continue;
        }
#endif
        // NOTE: Generation Delay for Tunks
        if (zox_getv(tunk->value, GenerateTunk)) {
            if (dbg_log) {
                zox_logw("Tunk Still Generating [%s]", zox_get_name(tunk->value));
            }
            continue;
        }
        const TownMap* town_map = zox_get(tunk->value, TownMap);
        byte lod = zox_getv(tunk->value, TunkLod);
        byte length = octree_size(lod);
#ifdef zox_safety_checks
        if (length * length != town_map->length) {
            zox_loge("Invalid [Tunk %s] [TownMap] Texture [%s] Size [%i]", zox_get_name(tunk->value), zox_get_name(e), town_map->length);
            continue;
        }
#endif
        size->value = int2_single(length);
        resize_TextureData(data, length * length);
        if (dbg_log) {
            zox_log("Generating Tunk Texture [%ix%i]", size->value.x, size->value.y);
        }
        for (byte x = 0; x < length; x++) {
            for (byte y = 0; y < length; y++) {
                int index = int2_array_index((int2) { x, y }, size->value);
                byte value = town_map->value[index];
                if (value == zox_town_type_none) {
                    data->value[index] = color_grayscale(32);
                } else if (value == zox_town_type_zone) {
                    data->value[index] = color_grayscale(88);
                } else if (value == zox_town_type_wall) {
                    data->value[index] = color_grayscale(222);
                } else if (value == zox_town_type_gate) {
                    data->value[index] = (color) { 222, 133, 133, 255 };
                } else if (value == zox_town_type_home) {
                    data->value[index] = color_grayscale(166);
                } else if (value == zox_town_type_home_wall || value == zox_town_type_home_door) {
                    data->value[index] = color_grayscale(189);
                } else {
                    data->value[index] = color_grayscale(128);
                }
            }
        }
        generate->value = 0;
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(TownTextureSystem);
