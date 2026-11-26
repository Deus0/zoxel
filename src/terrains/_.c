/*
 * +------------------------------------------------------+
 * | Zox Module: Terrains                                 |
 * |                                                      |
 * |  Voxel and poly terrains                             |
 * |                                                      |
 * +------------------------------------------------------+
 */

#include "voxel/_.c"

void import_terrains(ecs* world) {
    zox_module(terrains);
    zox_add_module(voxel_terrains);
}
