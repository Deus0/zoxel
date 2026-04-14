#if !defined(zoxm_cameras3) && defined(zoxm_transforms3)
#define zoxm_cameras3

#include "pre/_.c"
#include "sys/_.c"

zox_begin_module(Cameras3)
    add_hook_spawn_prefabs(spawn_prefabs_cameras3);
    define_systems_cameras3(world);
zox_end_module(Cameras3)

#endif
