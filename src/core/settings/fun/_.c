#include "save.c"

/*void save_settings();

// # New Settings #
void zoxs_set(const char* name, byte type, on_set_event on_set) {
    if (settings_count >= max_settings) {
        zox_log_error("settings count already at its limit of [%i]", max_settings)
        return;
    }
    setting value = {
        .name = name,
        .type = type,
        .on_set = on_set,
    };
    settings[settings_count] = value;
    settings_count++;
}

setting zoxs_get(const char *name) {
    for (uint i = 0; i < settings_count; i++) {
        setting setting = settings[i];
        if (!strcmp(name, setting.name)) {
            // zox_log("found %s at [%i]", name, i)
            return setting;
        }
    }
    return (setting) { 0 };
}*/

/*#include "byte.c"
#include "float.c"
#include "int.c"
#include "string.c"
#include "load.c"*/
