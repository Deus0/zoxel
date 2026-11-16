#include "hierarchy.c"

void zox_tsts_transforms(ecs* world) {
    zox_tst_add("transform_hierarchy", zox_tst_transform_hierarchy_ready, zox_tst_transform_hierarchy_check, 30);
    zox_tst_add("euler_limits", zox_tst_euler_limits_ready, zox_tst_euler_limits_check, 30);
}
