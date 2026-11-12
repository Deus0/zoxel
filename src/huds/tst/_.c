#include "load.c"
#include "menus.c"
#include "end_game.c"
#include "mouse.c"
#include "tooltips.c"
#include "options.c"
#include "scroll.c"
#include "exit.c"
#include "list.c"
#include "layout.c"
#include "font.c"

void zox_tsts_huds() {
    zox_tsts_huds_load();
    zox_tst_add("menu_flow", zox_tst_menu_flow_ready, zox_tst_menu_flow_check, 120);
    zox_tst_add("end_game_cleanup", zox_tst_end_game_ready, zox_tst_end_game_check, 180);
    zox_tst_set_setup("end_game_cleanup", NULL, 1);
    zox_tst_set_setup("menu_flow", NULL, 1);
    zox_tst_add("mouse_click", zox_tst_mouse_click_ready, zox_tst_mouse_click_check, 60);
    zox_tst_set_setup("mouse_click", NULL, 1);
    zox_tst_add("ui_leaks", zox_tst_ui_leaks_ready, zox_tst_ui_leaks_check, 120);
    zox_tst_set_setup("ui_leaks", NULL, 1);
    zox_tst_add("tooltips", zox_tst_tooltips_ready, zox_tst_tooltips_check, 60);
    zox_tst_set_setup("tooltips", NULL, 1);
    zox_tst_add("options_toggle", zox_tst_options_toggle_ready, zox_tst_options_toggle_check, 60);
    zox_tst_set_setup("options_toggle", NULL, 1);
    zox_tst_add("scroll_wheel", zox_tst_scroll_wheel_ready, zox_tst_scroll_wheel_check, 60);
    zox_tst_set_setup("scroll_wheel", NULL, 1);
    zox_tst_add("exit_menu", zox_tst_exit_menu_ready, zox_tst_exit_check, 60);
    zox_tst_set_setup("exit_menu", NULL, 1);
    zox_tst_set_exits("exit_menu");
    zox_tst_add("exit_game", zox_tst_exit_game_ready, zox_tst_exit_check, 90);
    zox_tst_set_exits("exit_game");
    zox_tst_add("list_order", zox_tst_list_order_ready, zox_tst_list_order_check, 60);
    zox_tst_set_setup("list_order", NULL, 1);
    zox_tst_add("ui_layout", zox_tst_ui_layout_ready, zox_tst_ui_layout_check, 90);
    zox_tst_set_setup("ui_layout", NULL, 1);
    zox_tst_add("font_glyphs", zox_tst_font_glyphs_ready, zox_tst_font_glyphs_check, 60);
    zox_tst_set_setup("font_glyphs", NULL, 1);
}
