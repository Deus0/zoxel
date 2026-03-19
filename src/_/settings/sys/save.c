// Grab all the settings and add to a single user file

zox_sys2(SettingSaveSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SettingDirty);

    byte dirty = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(SettingDirty, state);

        if (state->value == zox_dirty_active) {
            dirty = 1;
            break;
        }
    }

    if (!dirty) {
        return;
    }

    zox_log("Saving Settings %i", it->count);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();

        if (zox_has(e, SettingByte)) {
            zox_geter_value(e, SettingByte, byte, value);

            zox_log("Saving Setting [%s] %i", zox_get_name(e), value);
        }
    }

} zox_sys_end(SettingSaveSystem);
