#include "opengl.c"
#include "mouse_focus.c"
#include "touch.c"
#ifdef zox_sdl3
    #include "alt_tab.c"
    #include "core.c"
#endif

void zox_tsts_sdl() {
    zox_tst_add("sdl_mouse_focus", zox_tst_sdl_mouse_focus_ready, zox_tst_sdl_mouse_focus_check, 30);
    zox_tst_set_setup("sdl_mouse_focus", NULL, 1);
    zox_tst_add("sdl_touch", zox_tst_sdl_touch_ready, zox_tst_sdl_touch_check, 30);
    zox_tst_set_setup("sdl_touch", NULL, 1);
#ifdef zox_sdl3
    zox_tst_add("sdl_alt_tab", zox_tst_sdl_alt_tab_ready, zox_tst_sdl_alt_tab_check, 60);
    zox_tst_set_setup("sdl_alt_tab", NULL, 1);
    zox_tst_add("sdl_window", zox_tst_sdl_window_ready, zox_tst_sdl_window_check, 30);
    zox_tst_set_setup("sdl_window", NULL, 1);
    zox_tst_add("sdl_resize", zox_tst_sdl_resize_ready, zox_tst_sdl_resize_check, 60);
    zox_tst_set_setup("sdl_resize", NULL, 1);
    zox_tst_add("sdl_quit", zox_tst_sdl_quit_ready, zox_tst_sdl_quit_check, 60);
    zox_tst_set_exits("sdl_quit");
#endif
}
