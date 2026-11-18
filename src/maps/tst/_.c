#include "map_pixel.c"
#include "split_huds.c"

void zox_tsts_maps(ecs* world) {
    zox_tsts_map_pixel(world);
    zox_tst_add("split_screen_huds", zox_tst_split_screen_huds_ready, zox_tst_split_screen_huds_check, 120);
    zox_tst_set_setup("split_screen_huds", zox_tst_split_screen_huds_setup, 0);
}
