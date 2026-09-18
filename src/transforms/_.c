/*
 * +------------------------------------------------------+
 * | Zox Module: Transforms                               |
 * |                                                      |
 * |  Position - Rotation - Scale - Hierarchy -  State    |
 * |                                                      |
 * +------------------------------------------------------+
 */
#include "com/_.c"
#include "hierarchy/_.c"
#include "transforms2/_.c"
#include "transforms3/_.c"
#include "layouts/_.c"

void import_transforms(ecs* world) {
    zox_module(transforms);
    zoxd_components_transforms(world);
    zox_add_module(hierarchys);
    zox_add_module(transforms2);
    zox_add_module(transforms3);
    zox_add_module(layouts);
}
