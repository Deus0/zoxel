// its settings, spawned on the app (InitializeEvent)
void streaming_settings(ecs* world, entity e) {
    setting_tags(world, spawn_setting_byte_slider(world, e, "Near RD", terrain_lod_near, (byte2) { terrain_lod_near_min, terrain_lod_near_max }), zox_id(SettingTabWorld), zox_id(SettingNearRD));
    setting_tags(world, spawn_setting_byte_slider(world, e, "Far RD", terrain_lod_far, (byte2) { terrain_lod_far_min, terrain_lod_far_max }), zox_id(SettingTabWorld), zox_id(SettingFarRD));
    setting_tags(world, spawn_setting_byte_slider(world, e, "Vertical RD", render_distance_y, (byte2) { render_distance_y_min, render_distance_y_slider_max }), zox_id(SettingTabWorld), zox_id(SettingVerticalRD));
#ifdef zox_debug_settings
    setting_tags(world, spawn_setting_byte(world, e, "No Lods", disable_terrain_lods), zox_id(SettingTabDebug), zox_id(SettingNoLods));
    setting_tags(world, spawn_setting_byte(world, e, "No Frustums", disable_frustum_culling), zox_id(SettingTabDebug), zox_id(SettingNoFrustums));
#endif
}

// a setting changed (its Dirty, added again each frame it changes: a slider dragged)
void streaming_settings_on_dirty(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        if (zox_has(e, SettingByte)) {
            byte value = zox_getv(e, SettingByte);
            if (dbg_log) {
                zox_log("Float Setting [%s] Set [%f]", name->value, value);
            }
            if (zox_has(e, SettingNoLods)) {
                disable_terrain_lods = value;
            } else if (zox_has(e, SettingNearRD)) {
                terrain_lod_near = value;
                if (terrain_lod_near > terrain_lod_far) {
                    terrain_lod_far = terrain_lod_near;
                }
            } else if (zox_has(e, SettingFarRD)) {
                terrain_lod_far = value;
                if (terrain_lod_far < terrain_lod_near) {
                    terrain_lod_near = terrain_lod_far;
                }
            } else if (zox_has(e, SettingVerticalRD)) {
                render_distance_y = value;
            } else if (zox_has(e, SettingNoFrustums)) {
                disable_frustum_culling = value;
                zox_set_enabled(chunk_frustum_system, !disable_frustum_culling);
            }
        }
    }
}

// spawned on the app (InitializeEvent), applied when one changes (an observer on Dirty)
void streaming_settings_events(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, streaming_settings);
    zox_on_add(
        streaming_settings_on_dirty,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Dirty,
    );
}
