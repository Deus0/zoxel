
// TODO: header_spawn_system, HeaderResizeSystem (when window is dirty)
byte window_corner_size = 0;
byte window_outline_size = 0;
byte header_corner_size = 0;
byte header_outline_size = 0;
byte close_button_corner_size = 0;
byte close_button_outline_size = 0;

#include "dat/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "eve/_.c"
#include "dbg/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_windows(ecs* world) {
    zox_module(windows);
    zox_components_windows(world);
    zox_systems_windows(world);
    zox_events_windows(world);
    add_hook_spawn_prefabs(spawn_prefabs_windows);
#ifdef zox_tests
    zox_tsts_windows();
#endif
}
