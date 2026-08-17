/*
 * +------------------------------------------------------------------+
 * | Zox Module: Networking                                           |
 * |                                                                  |
 * |  Sockets - Packets - Connections - Transport - Synchronization   |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#ifndef zoxm_networking
#define zoxm_networking

// TODO: Detect Disconnection on client or host - pings
// TODO:
// TODO: Add new client queue - respond to their message after confirming and spawning
#include "dat/_.c"
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Networking) {
    zoxd_components_networking(world);
    zoxd_systems_networking(world);
    add_hook_terminal_command(process_arguments_networking);
    add_hook_on_boot(on_boot_networking);
    add_hook_spawn_prefabs(spawn_prefabs_networking);
} zox_end_module(Networking);

#endif
