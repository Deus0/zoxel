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
#if !defined(zoxm_markers) && defined(zoxm_quests)
#define zoxm_markers

// #include "com/_.c"
#include "sys/_.c"

zox_begin_module(Markers) {
    zox_define_systems_markers(world);
} zox_end_module(Markers);

#endif
