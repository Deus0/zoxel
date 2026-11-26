#include "stack.c"
#include "docks.c"
#include "scroll_overlap.c"

static void zox_tsts_windows(void) {
    zox_tst_add("window_docks", zox_tst_window_docks_ready, zox_tst_window_docks_check, 30);
    zox_tst_set_setup("window_docks", NULL, 1);
    zox_tst_add("window_stack_input", zox_tst_window_stack_ready, zox_tst_ui_fixture_check, 30);
    zox_tst_set_setup("window_stack_input", NULL, 1);
    zox_tst_add("scrollview_overlap", zox_tst_scroll_overlap_ready, zox_tst_ui_fixture_check, 30);
    zox_tst_set_setup("scrollview_overlap", NULL, 1);
}
