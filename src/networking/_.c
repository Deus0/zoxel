/*
 * +------------------------------------------------------------------+
 * | Zox Module: Networking                                           |
 * |                                                                  |
 * |  Sockets - Packets - Connections - Transport - Synchronization   |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
 *  TODO:
 *
 *      - Safe Packets for Connections
 *      - Pinging
 *      - Detect Disconnection on client or host
 *      - Add new client queue
 *          - respond to their message after confirming and spawning
 *
 */
#include "dat/_.c"
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_networking(ecs* world) {
    zox_module(networking);
    zoxd_components_networking(world);
    zoxd_systems_networking(world);
    add_hook_terminal_command(process_arguments_networking);
    add_hook_on_boot(on_boot_networking);
    add_hook_spawn_prefabs(spawn_prefabs_networking);
}