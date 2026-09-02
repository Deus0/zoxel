
// TODO: HeaderSpawnSystem, HeaderResizeSystem (when window is dirty)
byte window_corner_size = 0;
byte window_outline_size = 0;
byte header_corner_size = 0;
byte header_outline_size = 0;
byte close_button_corner_size = 0;
byte close_button_outline_size = 0;

#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_windows(ecs* world) {
    zox_module(windows);
    define_components_windows(world);
    define_systems_windows(world);
    add_hook_spawn_prefabs(spawn_prefabs_windows);
}
