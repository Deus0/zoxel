#include "ui_mask.c"

static void zox_tsts_masks(void) {
    zox_tst_add("ui_mask", zox_tst_ui_mask_ready, zox_tst_ui_fixture_check, 20);
    zox_tst_set_setup("ui_mask", NULL, 1);
}
