#include "deferred_spawn.c"
#include "hidden_row.c"

static void zox_tsts_elements3(void) {
    zox_tst_add("ui3_deferred_spawn", zox_tst_ui3_deferred_ready, zox_tst_ui_fixture_check, 30);
    zox_tst_set_setup("ui3_deferred_spawn", NULL, 1);
    zox_tst_add("ui3_hidden_row_relabel", zox_tst_ui3_hidden_row_ready, zox_tst_ui_fixture_check, 30);
    zox_tst_set_setup("ui3_hidden_row_relabel", NULL, 1);
}
