/*
 *  Zox Sky
 *      - Clouds: flat blocky boxes high above the terrain, children of the realm, drifting with the wind
 *        and wrapping round the camera that hears sounds (SoundListener), so the sky is never empty
 */
#include "com/_.c"
#include "pre/_.c"
#include "sys/_.c"

void import_sky(ecs* world) {
    zox_module(sky);
    zox_components_sky(world);
    zox_systems_sky(world);
    add_hook_load_shader(&zox_shaders_sky);
    add_hook_spawn_prefabs(zox_prefabs_sky);
}
