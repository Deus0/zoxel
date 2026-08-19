zox_sys2(StreamingSettingsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LoadSettings);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LoadSettings, load);
        if (load->value != zox_load_settings_spawn) {
            continue;
        }
        spawn_setting_byte_slider(world, e, "Near RD", terrain_lod_near, (byte2) { terrain_lod_near_min, terrain_lod_near_max });
        spawn_setting_byte_slider(world, e, "Far RD", terrain_lod_far, (byte2) { terrain_lod_far_min, terrain_lod_far_max });
        spawn_setting_byte_slider(world, e, "Vertical RD", render_distance_y, (byte2) { render_distance_y_min, render_distance_y_max });
#ifdef zox_debug_settings
        spawn_setting_byte(world, e, "No Lods", disable_terrain_lods);
        spawn_setting_byte(world, e, "No Frustums", disable_frustum_culling);
#endif
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
            if (!strcmp(name->value, "No Lods")) {
                disable_terrain_lods = value;
            } else if (!strcmp(name->value, "Near RD")) {
                terrain_lod_near = value;
                if (terrain_lod_near > terrain_lod_far) {
                    terrain_lod_far = terrain_lod_near;
                }
            } else if (!strcmp(name->value, "Far RD")) {
                terrain_lod_far = value;
                if (terrain_lod_far < terrain_lod_near) {
                    terrain_lod_near = terrain_lod_far;
                }
            } else if (!strcmp(name->value, "Vertical RD")) {
                render_distance_y = value;
            } else if (!strcmp(name->value, "No Frustums")) {
                disable_frustum_culling = value;
                zox_set_enabled(ChunkFrustumSystem, !disable_frustum_culling);
            }
        }
    }
} zox_sys_end(StreamingSettingsDirtySystem);
