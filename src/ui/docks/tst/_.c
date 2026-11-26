#include "docks.c"

static void zox_tsts_docks(void) {
    zox_tst_add("window_docks", zox_tst_docks_ready, zox_tst_docks_check, 30);
    zox_tst_set_setup("window_docks", NULL, 1);
}
