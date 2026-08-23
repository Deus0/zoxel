/*
 * +------------------------------------------------------------------+
 * | Zox Module: Heights                                              |
 * |                                                                  |
 * |  Elevation - Upwards                                             |
 * |                                                                  |
 * |  Uses: Tunks                                                     |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
byte zox_disable_heights = 0;

#include "com/_.c"
#include "sys/_.c"

void import_heights(ecs* world) {
    zox_module(heights);
    zox_systems_heights(world);
}
