// NOTE: Simply creates a height texture from tunks
zox_sys2(TunkTextureSystem) {
    byte dbg_log = 0;
    byte map_type = 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(TunkLink);
    zox_sys_out(TextureData);
    zox_sys_out(TextureSize);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(TunkLink, tunk);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureSize, size);
        zox_sys_o(TextureDirty, dirty);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        // NOTE: Validate Tunks
        if (!zox_valid(tunk->value) || !zox_has(tunk->value, Generate) || !zox_has(tunk->value, HeightMap)) {
            // zox_loge("Invalid [Tunk] for Texture [%s]", zox_get_name(e));
            size->value = int2_single(0);
            resize_TextureData(data, size->value.x * size->value.y);
            dirty->value = zox_dirty_trigger;
            continue;
        }
        entity terrain = zox_get_parent(world, tunk->value);
        // NOTE: Validate Terrain
        if (!zox_valid(terrain) || !zox_has(terrain, NodeDepth)) {
            zox_loge("Invalid [Terrain] for Texture [%s]", zox_get_name(e));
            continue;
        }
        // NOTE: Generation Delay for Tunks
        if (zox_getv(tunk->value, Generate)) {
            if (dbg_log) {
                zox_logw("Tunk Still Generating [%s]", zox_get_name(tunk->value));
            }
            zox_set(e, Generate, { zox_dirty_trigger });
            continue;
        }
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        int map_length = powers_of_two[terrain_depth];
        size->value = int2_single(map_length);
        const HeightMap* height_map = zox_get(tunk->value, HeightMap);
        if (map_length * map_length != height_map->length) {
            zox_loge("Invalid [Tunk] [Heightmap] for Texture [%s]", zox_get_name(e));
            continue;
        }
        if (dbg_log) {
            zox_log("Generating Tunk Texture [%ix%i]", size->value.x, size->value.y);
        }
        resize_TextureData(data, size->value.x * size->value.y);
        for (byte x = 0; x < map_length; x++) {
            for (byte y = 0; y < map_length; y++) {
                int index = int2_array_index((int2) { x, y }, size->value);
                byte value = height_map->value[index];
                value = int_clamp(32 + value * 10, 0, 255);
                data->value[index] = color_grayscale(value);
                if (dbg_log >= 2) {
                    zox_log(" - [%ix%i]%i: %i", x, y, index, value);
                }
            }
        }
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(TunkTextureSystem);
