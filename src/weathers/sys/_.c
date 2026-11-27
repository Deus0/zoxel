#include "skybox_set_time_system.c"
#include "sky_blend.c"

// SkyBlendStart: the sky's blend to its SkyColorTarget started (blend_sky_to), on the main thread at the merge: its
//  tween is an entity, workers can't make one (biome_link_system sets it from them)
void sky_blend_start_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity e = it->entities[i];
        blend_sky_to(world, e, zox_getv(e, SkyColorTarget));
        zox_remove(e, SkyBlendStart);
    }
}

void zox_systems_weather(ecs* world) {
    zox_on_add(sky_blend_start_event, [none] weathers.SkyBlendStart);
    zox_system_1(
        skybox_set_time_system,
        zoxp_gpu_upload,
        [in] rendering.MaterialGPULink,
        [none] Skybox
    );
    zox_system_1(
        sky_blend_system,
        zoxp_gpu_upload,
        [in] SkyBlend,
        [in] SkyColorFrom,
        [in] SkyColorTarget,
        [in] colorz.ColorRGB,
        [none] Skybox
    );
}
