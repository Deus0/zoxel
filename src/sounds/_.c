/*
 * +------------------------------------------------------------------+
 * | Zox Module: Sounds                                               |
 * |                                                                  |
 * |  Effects - Instruments - Voices - Channels                       |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#define zox_disable_sound_pool

#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "io/_.c"
#include "wav/_.c"
#include "fun/_.c"
#include "bop/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_sounds(ecs* world) {
    zox_module(sounds);
    define_components_sounds(world);
    define_systems_sounds(world);
    zox_module_dispose(dispose_sounds)
    add_hook_terminal_command(process_arguments_sounds);
    add_hook_files_load(load_files_sounds);
    add_hook_spawn_prefabs(spawn_prefabs_sounds);
}