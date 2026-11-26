/*
 * +------------------------------------------------------+
 * | Zox Module: Terrains                                 |
 * |                                                      |
 * |  Voxel and poly terrains                             |
 * |                                                      |
 * +------------------------------------------------------+
 */

#include "voxel/_.c"
#include "poly/_.c"

void import_terrains(ecs* world) {
    zox_module(terrains);
    zox_add_module(voxel_terrains);
    zox_add_module(poly_terrains);
}
