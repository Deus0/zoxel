// relies on players and cameras
#if !defined(zoxm_free_roam) && defined(zoxm_cameras) && defined(zoxm_players)
#define zoxm_free_roam

#include "dat/settings.c"
#include "sys/_.c"

zox_begin_module(FreeRoam)
    define_systems_free_roam(world);
zox_end_module(FreeRoam)

#endif

