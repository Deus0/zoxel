zox_sys2(FirstTerrainTunkSystem) {
    if (zox_cameras_disable_streaming) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StreamLink);
    zox_sys_in(StreamPosition2);
    zox_sys_in(StreamDirty2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(StreamLink, terrain);
        zox_sys_i(StreamPosition2, position);
        zox_sys_i(StreamDirty2, dirty);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(terrain->value) || !zox_has(terrain->value, TunkLinks)) {
            continue;
        }
        zox_mut_begin(terrain->value, TunkLinks, tunks);
        if (int2_hashmap_has(tunks->value, position->value)) {
            continue;
        }
        entity tunk = spawn_tunk(world, prefab_tunk2, terrain->value, position->value, 0);
        if (!zox_valid(tunk)) {
            zox_loge("Failed to spawn Tunk [%ix%i] on %s", position->value.x, position->value.y, zox_get_name(terrain->value));
            continue;
        }
        int2_hashmap_add(tunks->value, position->value, tunk);
        zox_mut_end(terrain->value, TunkLinks);
    }
} zox_sys_end(FirstTerrainTunkSystem);
