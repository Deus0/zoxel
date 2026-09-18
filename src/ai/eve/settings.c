void ai_settings(ecs* world, entity app) {
        spawn_setting_byte(world, app, "No Humans", zox_no_humanoids);
#ifdef zox_debug_settings
        // spawn_setting_byte(world, app, "No Npcs", disable_npcs);
#endif
}

void ai_settings_on_dirty(iter* it) {
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
                zox_log("SettingByte [%s] Set [%f]",
                    name->value,
                    value);
            }
            /*if (!strcmp(name->value, "No Npcs")) {
                disable_npcs = value;
            } else */
            if (!strcmp(name->value, "No Humans")) {
                zox_no_humanoids = value;
            }
        }
    }
}

