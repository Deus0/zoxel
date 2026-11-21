#include "sea.c"
#include "town_maps.c"
#include "town_build.c"
#include "aesthetic.c"
void zox_tsts_towns(ecs* world) {
    zox_tst_add("sea_level", zox_tst_sea_level_ready, zox_tst_sea_level_check, 120);
    zox_tst_set_setup("sea_level", NULL, 1);
    zox_tst_add("lakes", zox_tst_lakes_ready, zox_tst_lakes_check, 120);
    zox_tst_set_setup("lakes", NULL, 1);
    zox_tsts_town_maps(world);
    zox_tsts_town_build(world);
    zox_tst_add("aesthetic_town", zox_tst_aesthetic_town_ready, zox_tst_aesthetic_town_check, 240);
    zox_tst_add("aesthetic_particles", zox_tst_aesthetic_particles_ready, zox_tst_aesthetic_particles_check, 240);
}
