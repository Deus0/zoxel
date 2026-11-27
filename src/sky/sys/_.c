#include "spawn.c"
#include "drift.c"
#include "camera_sky.c"

void zox_systems_sky(ecs* world) {
    zox_system_1(
        clouds_spawn_system,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [none] sounds.SoundListener
    );
    zox_system(
        cloud_drift_system,
        zoxp_physics,
        [out] transforms3.Position3D,
        [none] Cloud
    );
    zox_system(
        camera_sky_system,
        zoxp_update,
        [out] cameras.SkyTint,
        [out] cameras.SunDirection,
        [out] cameras.SunColor,
        [none] cameras.Camera
    );
}
