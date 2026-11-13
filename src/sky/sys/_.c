#include "spawn.c"
#include "drift.c"

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
}
