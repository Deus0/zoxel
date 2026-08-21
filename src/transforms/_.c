/*
 * +------------------------------------------------------------------+
 * | Zox Module: Transforms                                           |
 * |                                                                  |
 * |  Position - Rotation - Scale - Hierarchy - Transform State       |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#include "com/_.c"
#include "hierarchy/_.c"
#include "transforms2/_.c"
#include "transforms3/_.c"
#include "layouts/_.c"

void import_transforms(ecs* world) {
    zox_module(transforms);
    zoxd_components_transforms(world);
    zox_import_module(Hierarchys);
    zox_import_module(Transforms2);
    zox_import_module(Transforms3);
    zox_add_module(layouts);
}
