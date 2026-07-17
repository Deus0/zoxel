// Load and apply each setting, firing on_set as needed
extern const char* game_name;

void load_files_settings(ecs* world) {
#ifdef zox_disable_load_settings
    return;
#endif
    const char* app_name = game_name;
    settings_game_name = app_name;
    const char* fn = get_settings_file(app_name);
    zox_logv("  - Loading Files Settings [%s]", fn);
    FILE* f = fopen(fn, "r");
    if (!f) {
        zox_logv("      - No settings File Found.");
        has_loaded = 1;
        return;
    }
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        // trim newline
        line[strcspn(line, "\r\n")] = 0;
        char* name = strtok(line, ":");
        char* type = strtok(NULL, ":");
        char* val  = strtok(NULL, "");
        if (!name || !type || !val) {
            continue;
        }
        if (strcmp(type, "byte") == 0) {
            byte v = (byte)atoi(val);
            // zoxs_set_byte(world, name, v);
            zox_log("- loaded byte [%s] [%i]", name, v);
        } else if (strcmp(type, "int") == 0) {
            int v = atoi(val);
            // zoxs_set_int(world, name, v);
            zox_log("- loaded int [%s] [%i]", name, v);
        } else if (strcmp(type, "float") == 0) {
            float v = strtof(val, NULL);
            // zoxs_set_float(world, name, v);
            zox_log("- loaded float [%s] [%f]", name, v);
        } else if (strcmp(type, "string") == 0) {
            // zoxs_set_string(world, name, val);
        }
    }
    fclose(f);
    has_loaded = 1;
}
