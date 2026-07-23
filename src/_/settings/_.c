// Zox settings

// TODO: Mark settings as dirty but not for saving, so the load can trigger the effects
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
