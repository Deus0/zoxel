/*
 * +------------------------------------------------------------------+
 * | Zox Module: Markers                                                |
 * |                                                                  |
 * |  Overheads - Icons - Labels - Targets - Marker State                |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
 *  TODO:
 *
 *      - Overhead icons for character signal
 *          - ! - Has Quest
 *          - ? - Hand in Quest
 *          - X - Enemy
 *
 * */
// #include "com/_.c"
#include "sys/_.c"

void import_markers(ecs* world) {
    zox_module(markers);
    zox_define_systems_markers(world);
}