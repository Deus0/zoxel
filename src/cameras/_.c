

// TODO: If Camera Distance > 500, use double precision for planes - it still flickers so we need to use for now
byte is_camera_positive_z = 1;
/*#if defined(zox_web)
    byte zox_disable_post_processing = 1;
#else
    byte zox_disable_post_processing = 0;
#endif*/
byte zox_disable_post_processing = 0;

#include "sta/_.c"
#include "dat/_.c"
#include "set/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "cameras2/_.c"
#include "cameras3/_.c"
#include "fun/player.c"
#include "dbg/_.c"

void import_cameras(ecs* world) {
    zox_module(cameras);
    zox_components_cameras(world);
    zox_systems_cameras(world);
    add_hook_spawn_prefabs(spawn_prefabs_cameras);
    zox_add_module(cameras2);
    zox_add_module(cameras3);
}
