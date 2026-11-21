
// a setting changed (its Dirty, added again each frame it changes: a slider dragged)
void vodes_settings_on_dirty(iter* it) {
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
            if (!strcmp(name->value, "No Vodes")) {
                zox_disable_vodes = value;
            }
        }
    }
}

// spawned on the app (InitializeEvent), applied when one changes (an observer on Dirty)
void vodes_settings_events(ecs* world) {
    zox_on_add(
        vodes_settings_on_dirty,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Dirty,
    );
}
