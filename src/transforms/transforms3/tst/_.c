static byte zox_tst_transform3_ready(ecs* world) {
    (void) world;
    return 1;
}

#include "hierarchy.c"
#include "scale_hierarchy.c"
#include "matrices.c"

static void zox_tsts_transforms3(void) {
    zox_tst_add("transform_hierarchy", zox_tst_transform_hierarchy_ready, zox_tst_transform_hierarchy_check, 30);
    zox_tst_add("euler_limits", zox_tst_euler_limits_ready, zox_tst_euler_limits_check, 30);
    zox_tst_add("transform3_scale_hierarchy", zox_tst_transform3_ready, zox_tst_transform3_scale_hierarchy_check, 20);
    zox_tst_set_setup("transform3_scale_hierarchy", NULL, 1);
    zox_tst_add("transform3_matrices", zox_tst_transform3_ready, zox_tst_transform3_matrices_check, 20);
    zox_tst_set_setup("transform3_matrices", NULL, 1);
}
