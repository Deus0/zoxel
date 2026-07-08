zox_sys2(StreamingSettingsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeEntity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(InitializeEntity, state);
        if (state->value != zox_dirty_active) {
            continue;
        }
        spawn_setting_byte(world, e, "Disable Lods", disable_terrain_lods);
        spawn_setting_byte_slider(world, e, "Near Distance", terrain_lod_near, (byte2) { terrain_lod_near_min, terrain_lod_near_max });
        spawn_setting_byte_slider(world, e, "Far Distance", terrain_lod_far, (byte2) { terrain_lod_far_min, terrain_lod_far_max });
    }
} zox_sys_end(StreamingSettingsSystem);

zox_sys2(StreamingSettingsDirtySystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SettingDirty);
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SettingDirty, state);
        zox_sys_i(ZoxName, name);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (zox_has(e, SettingByte)) {
            byte value = zox_getv(e, SettingByte);
            if (dbg_log) {
                zox_log("Float Setting [%s] Set [%f]", name->value, value);
            }
            if (!strcmp(name->value, "Disable Lods")) {
                disable_terrain_lods = value;
            } else if (!strcmp(name->value, "Near Distance")) {
                terrain_lod_near = value;
                if (terrain_lod_near > terrain_lod_far) {
                    terrain_lod_far = terrain_lod_near;
                }
            } else if (!strcmp(name->value, "Far Distance")) {
                terrain_lod_far = value;
                if (terrain_lod_far < terrain_lod_near) {
                    terrain_lod_near = terrain_lod_far;
                }
            }
        }
    }
} zox_sys_end(StreamingSettingsDirtySystem);
