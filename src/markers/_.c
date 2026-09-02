/*
 * +--------------------------------------------------------+
 * | Zox Module: Markers                                    |
 * |                                                        |
 * |  Overheads - Icons - Labels - Targets - Marker State   |
 * |                                                        |
 * +--------------------------------------------------------+
 *
 *  TODO:
 *
 *      - Overhead icons for character signal
 *          - ! - Has Quest
 *          - ? - Hand in Quest
 *          - X - Enemy
 *
 * */
#include "com/_.c"
#include "sys/_.c"
#include "eve/_.c"

void import_markers(ecs* world) {
    zox_module(markers);
    zox_components_markers(world);
    zox_systems_markers(world);
    add_hook_spawn_prefabs(zox_events_markers);
}
