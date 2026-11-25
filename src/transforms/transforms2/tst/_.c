static byte zox_tst_transform2_ready(ecs* world) {
    (void) world;
    return 1;
}

#include "hierarchy.c"
#include "matrices.c"

static void zox_tsts_transforms2(void) {
    zox_tst_add("transform2_hierarchy", zox_tst_transform2_ready, zox_tst_transform2_hierarchy_check, 20);
    zox_tst_set_setup("transform2_hierarchy", NULL, 1);
    zox_tst_add("transform2_matrices", zox_tst_transform2_ready, zox_tst_transform2_matrices_check, 20);
    zox_tst_set_setup("transform2_matrices", NULL, 1);
}
