// Grab all the settings and add to a single user file
// TODO: Do this per app, its so weird seeing this monstrosity
zox_sys2(SettingSaveSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SettingDirty);
    entity dirty_app = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SettingDirty, state);
        if (state->value == zox_dirty_active) {
            dirty_app = zox_get_parent(world, e);
            break;
        }
    }
    if (!dirty_app) {
        return;
    }
    const char* app_name = settings_game_name;
    const char* filepath = get_settings_file(app_name);
    FILE* file = fopen(filepath, "w");
    if (!file) {
        zox_loge("failed to open settings file [%s]: %s", filepath, strerror(errno));
        return;
    }
    if (dbg_log) zox_log("Saving Settings [%s]: %i ", filepath, it->count);
    // dirty_app
    iter it2 = zox_children(world, dirty_app);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e = it2.entities[j];
            if (!zox_has(e, Setting)) {
                continue;
            }
            const ZoxName* name = zox_get(e, ZoxName);
            if (zox_has(e, SettingByte)) {
                byte value = zox_getv(e, SettingByte);
                fprintf(file, "%s:byte:%u\n", name->value, value);
                if (dbg_log) {
                    zox_log("   - Saved Byte [%s] %i", name->value, value);
                }
            } else if (zox_has(e, SettingInt)) {
                int value = zox_getv(e, SettingInt);
                fprintf(file, "%s:int:%d\n", name->value, value);
                if (dbg_log) {
                    zox_log("   - Saved Int [%s] [%i]", name->value, value);
                }
            } else if (zox_has(e, SettingFloat)) {
                float value = zox_getv(e, SettingFloat);
                fprintf(file, "%s:float:%.01f\n", name->value, value);
                if (dbg_log) {
                    zox_log("   - Saved Float [%s] [%i]", name->value, value);
                }
            }
        }
        //  fprintf(f, "%s:string:%s\n",s.name, s.value_string);
    }
    fclose(file);
} zox_sys_end(SettingSaveSystem);
