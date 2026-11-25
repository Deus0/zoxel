#include "transforms.c"
#include "test_canvas.c"
#ifdef zox_tests
#include "shared_canvas.c"
#ifdef zox_sdl
#include "canvas_resize.c"
#endif

void zox_tsts_ui(ecs* world) {
    zox_tst_add("shared_canvas_input", zox_tst_shared_canvas_ready, zox_tst_ui_fixture_check, 30);
    zox_tst_set_setup("shared_canvas_input", NULL, 1);
#ifdef zox_sdl
    zox_tst_add("canvas_resize_input", zox_tst_canvas_resize_ready, zox_tst_ui_fixture_check, 30);
    zox_tst_set_setup("canvas_resize_input", NULL, 1);
#endif
}
#endif
