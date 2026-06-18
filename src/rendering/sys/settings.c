zox_sys2(RenderingSettingsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeEntity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(InitializeEntity, state);
        if (state->value != zox_dirty_active) {
            continue;
        }
        spawn_setting_byte_slider(world, e, "Downscale", viewport_downscale, (byte2) { 1, 8 });
        spawn_setting_float(world, e, "Fog", fog_density / 0.068f, (float2) { 0, 1 });
    }
} zox_sys_end(RenderingSettingsSystem);

zox_sys2(RenderingSettingsDirtySystem) {
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
        if (zox_has(e, SettingFloat)) {
            float value = zox_getv(e, SettingFloat);
            if (dbg_log) {
                zox_log("Float Setting [%s] Set [%f]", name->value, value);
            }
            if (!strcmp(name->value, "Fog")) {
                fog_density = 0.068f * value;
            }
        } else if (zox_has(e, SettingByte)) {
            byte value = zox_getv(e, SettingByte);
            if (dbg_log) {
                zox_log("Byte Setting [%s] Set [%i]", name->value, value);
            }
            if (!strcmp(name->value, "Downscale")) {
                viewport_downscale = value;
                viewport_scale = 1 / (float) viewport_downscale;
                // TODO: Apply to actual viewport??
                zox_set(main_app, WindowSizeDirty, { zox_dirty_trigger });
                entity canvas = zox_get_child_by_id(world, main_app, zox_id(Canvas));
                entity render_texture = zox_get_child_by_id(world, canvas, zox_id(RenderTexture));
                zox_set(render_texture, LayoutSizeDirty, { zox_dirty_trigger });
            }
        }
    }
} zox_sys_end(RenderingSettingsDirtySystem);
