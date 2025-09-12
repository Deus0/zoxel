#ifndef zoxm_networking
#define zoxm_networking

byte zox_log_network_errors = 0;
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Networking)
    add_hook_terminal_command(process_arguments_networking);
    add_hook_on_boot(on_boot_networking);
    add_hook_spawn_prefabs(spawn_prefabs_networking);
    define_components_networking(world);
    define_systems_networking(world);
zox_end_module(Networking)

#endif
