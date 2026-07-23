#define MAX_SETTINGS_LINES 1024

byte set_app_setting_byte(ecs* world, entity e, const char* name, byte value) {
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int i = 0; i < it2.count; i++) {
            entity e2 = it2.entities[i];
            if (!zox_has(e2, SettingByte)) {
                continue;
            }
            const ZoxName* setting_name = zox_get(e2, ZoxName);
            if (!strcmp(setting_name->value, name)) {
                zox_setm(e2, SettingByte, value);
                zox_set(e2, SettingDirty, { zox_dirty_trigger });
                return 1;
            }
        }
    }
    return 0;
}

byte set_app_setting_int(ecs* world, entity e, const char* name, int value) {
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int i = 0; i < it2.count; i++) {
            entity e2 = it2.entities[i];
            if (!zox_has(e2, SettingInt)) {
                continue;
            }
            const ZoxName* setting_name = zox_get(e2, ZoxName);
            if (!strcmp(setting_name->value, name)) {
                zox_setm(e2, SettingInt, value);
                zox_set(e2, SettingDirty, { zox_dirty_trigger });
                return 1;
            }
        }
    }
    return 0;
}

byte set_app_setting_float(ecs* world, entity e, const char* name, float value) {
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int i = 0; i < it2.count; i++) {
            entity e2 = it2.entities[i];
            if (!zox_has(e2, SettingFloat)) {
                continue;
            }
            const ZoxName* setting_name = zox_get(e2, ZoxName);
            if (!strcmp(setting_name->value, name)) {
                zox_setm(e2, SettingFloat, value);
                zox_set(e2, SettingDirty, { zox_dirty_trigger });
                return 1;
            }
        }
    }
    return 0;
}

// Loads a simple settings file for users
zox_sys2(SettingsLoadSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    zox_sys_out(LoadSettings);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        zox_sys_o(LoadSettings, load);
        if (load->value == zox_load_settings_start) {
            load->value = zox_load_settings_spawn;
            continue;
        }
        if (load->value == zox_load_settings_spawn) {
            load->value = zox_load_settings_run;
            continue;
        }
        if (load->value != zox_load_settings_run) {
            continue;
        }
#ifdef zox_disable_load_settings
        load->value = zox_load_settings_end;
        continue;
#endif
        const char* app_name = name->value; // game_name;
        settings_game_name = app_name;
        const char* filepath = get_settings_file(app_name);
        if (dbg_log) {
            zox_log("Loading Files Settings at [%s]", filepath);
        }
        FILE* file = fopen(filepath, "r");
        if (!file) {
            if (dbg_log) {
                zox_log("No settings File Found.");
            }
            load->value = zox_load_settings_end;
            continue;
        }
        char line[MAX_SETTINGS_LINES];
        while (fgets(line, sizeof(line), file)) {
            // trim newline
            line[strcspn(line, "\r\n")] = 0;
            char* name = strtok(line, ":");
            char* type = strtok(NULL, ":");
            char* raw  = strtok(NULL, "");
            if (!name || !type || !raw) {
                continue;
            }
            if (strcmp(type, "byte") == 0) {
                byte value = (byte)atoi(raw);
                set_app_setting_byte(world, e, name, value);
                if (dbg_log)
                    zox_log("- loaded byte [%s] [%i]", name, value);
            } else if (strcmp(type, "int") == 0) {
                int value = atoi(raw);
                set_app_setting_int(world, e, name, value);
                if (dbg_log)
                    zox_log("- loaded int [%s] [%i]", name, value);
            } else if (strcmp(type, "float") == 0) {
                float value = strtof(raw, NULL);
                set_app_setting_float(world, e, name, value);
                if (dbg_log)
                    zox_log("- loaded float [%s] [%f]", name, value);
            } else if (strcmp(type, "string") == 0) {
                // zoxs_set_string(world, name, raw);
            }
        }
        fclose(file);
        load->value = zox_load_settings_end;
    }
} zox_sys_end(SettingsLoadSystem);
