#include "prefab.c"
#include "motion.c"
#include "lifetime.c"

static void zox_tsts_particles2(void) {
    zox_tst_add("particle2_prefab", zox_tst_particle2_ready, zox_tst_particle2_prefab_check, 30);
    zox_tst_set_setup("particle2_prefab", NULL, 1);
    zox_tst_add("particle2_motion", zox_tst_particle2_motion_ready, zox_tst_particle2_motion_check, 30);
    zox_tst_set_setup("particle2_motion", NULL, 1);
    zox_tst_add("particle2_lifetime", zox_tst_particle2_lifetime_ready, zox_tst_particle2_lifetime_check, 30);
    zox_tst_set_setup("particle2_lifetime", NULL, 1);
}
