// its settings, spawned on the app (InitializeEvent)
void rendering_settings(ecs* world, entity e) {
    spawn_setting_float(world, e, "Fog", fog_density / fog_settings_mul, (float2) { 0, 1 });
    spawn_setting_byte_slider(world, e, "Downscale", viewport_downscale, (byte2) { 1, 8 });
}

extern void on_set_viewport_scale(ecs*, entity);

// a setting changed (Update, the frame after its Dirty)
void rendering_settings_on_dirty(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        if (zox_has(e, SettingFloat)) {
            float value = zox_getv(e, SettingFloat);
            if (dbg_log) {
                zox_log("Float Setting [%s] Set [%f]", name->value, value);
            }
            if (!strcmp(name->value, "Fog")) {
                fog_density = value * fog_settings_mul;
            }
        } else if (zox_has(e, SettingByte)) {
            byte value = zox_getv(e, SettingByte);
            if (dbg_log) {
                zox_log("Byte Setting [%s] Set [%i]", name->value, value);
            }
            if (!strcmp(name->value, "Downscale")) {
                viewport_downscale = value;
                entity app = zox_get_parent(world, e);
                on_set_viewport_scale(world, app);
            }
        }
    }
}

// spawned on the app (InitializeEvent), applied when one changes (an observer on Update)
void rendering_settings_events(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, rendering_settings);
    zox_on_add(
        rendering_settings_on_dirty,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Update,
    );
}
