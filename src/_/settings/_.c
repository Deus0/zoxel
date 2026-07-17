// Zox settings
//
// void on_set_float(ecs*world, void* value) {
//     float valuef = *(float*) value;
//     zox_log("> brightness was set to %f", valuef)
// }
//
// zox_begin_module(ModuleName)
//     zoxs_set("brightness", zox_data_type_float, &on_brightness_set);
//     zoxs_set_float("brightness", 0.75f);
//     setting s = zoxs_get("brightness");
//     zox_log("Current brightness: %f\n", zoxs_get_float(s));
// zox_end_module(ModuleName)
//
#include "dat/_.c"
#define max_settings 256
uint settings_count = 0;
setting settings[max_settings];
#include "com/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Settings) {
    zox_define_components_settings(world);
    define_systems_settings(world);
    add_hook_spawn_prefabs(spawn_prefabs_settings);
} zox_end_module(Settings);
