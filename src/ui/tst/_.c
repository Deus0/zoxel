#include "transforms.c"
#include "test_canvas.c"
#include "helpers.c"
#ifdef zox_tests
#include "tooltip_gone.c"

void zox_tsts_ui(ecs* world) {
    zox_tsts_tooltip_gone(world);
}
#endif
