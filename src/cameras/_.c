#if !defined(zoxm_cameras) && defined(zoxm_transforms)
#define zoxm_cameras

// TODO: If Camera Distance > 500, use double precision for planes - it still flickers so we need to use for now
#define frustumdouble
byte is_camera_positive_z = 1;

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

zox_begin_module(Cameras) {
    define_components_cameras(world);
    define_systems_cameras(world);
    add_hook_spawn_prefabs(spawn_prefabs_cameras);
    zox_import_module(Cameras2);
    zox_import_module(Cameras3);
} zox_end_module(Cameras);

#endif
