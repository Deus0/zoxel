#include "map_pixel.c"
#include "split_huds.c"
#include "markers.c"

void zox_tsts_maps(ecs* world) {
    zox_tsts_map_pixel(world);
    zox_tst_add("split_screen_huds", zox_tst_split_screen_huds_ready, zox_tst_split_screen_huds_check, 120);
    zox_tst_set_setup("split_screen_huds", zox_tst_split_screen_huds_setup, 0);
    zox_tst_add("coop_map_markers", zox_tst_coop_map_markers_ready, zox_tst_coop_map_markers_check, 120);
    zox_tst_set_setup("coop_map_markers", zox_tst_split_screen_huds_setup, 0);
}
