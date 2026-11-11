#include "load.c"
#include "menus.c"
#include "mouse.c"
#include "tooltips.c"
#include "options.c"

void zox_tsts_huds() {
    zox_tsts_huds_load();
    zox_tst_add("menu_flow", zox_tst_menu_flow_ready, zox_tst_menu_flow_check, 120);
    zox_tst_set_setup("menu_flow", NULL, 1);
    zox_tst_add("mouse_click", zox_tst_mouse_click_ready, zox_tst_mouse_click_check, 60);
    zox_tst_set_setup("mouse_click", NULL, 1);
    zox_tst_add("ui_leaks", zox_tst_ui_leaks_ready, zox_tst_ui_leaks_check, 120);
    zox_tst_set_setup("ui_leaks", NULL, 1);
    zox_tst_add("tooltips", zox_tst_tooltips_ready, zox_tst_tooltips_check, 60);
    zox_tst_set_setup("tooltips", NULL, 1);
    zox_tst_add("options_toggle", zox_tst_options_toggle_ready, zox_tst_options_toggle_check, 60);
    zox_tst_set_setup("options_toggle", NULL, 1);
}
