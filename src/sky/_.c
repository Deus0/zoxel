/*
 *  Zox Sky
 *      - Clouds: flat blocky boxes high above the terrain, children of the realm, drifting with the wind
 *        and wrapping round the camera that hears sounds (SoundListener), so the sky is never empty
 *      - Suns: light sources, children of each realm, their day by the realm's clock; each camera's sky light from
 *        its realm's (doc/plans/day_night.md)
 */
#include "com/_.c"
#include "fun/sun.c"
#include "pre/_.c"
#include "sys/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_sky(ecs* world) {
    zox_module(sky);
    zox_components_sky(world);
    zox_systems_sky(world);
    add_hook_load_shader(&zox_shaders_sky);
    add_hook_spawn_prefabs(zox_prefabs_sky);
#ifdef zox_tests
    zox_tsts_sky(world);
#endif
}
