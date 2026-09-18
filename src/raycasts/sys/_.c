#include "camera_rays.c"
#include "debug_rays.c"

void zox_systems_raycasts(ecs* world) {
    zox_system(
        CameraRaySystem,
        zoxp_update,
        [out] raycasts.RaycastOrigin,
        [out] raycasts.RaycastNormal
    );
    zox_system_1(
        RayDebugSystem,
        zoxp_spawn,
        [in] raycasts.RaycastOrigin,
        [in] raycasts.RaycastNormal
    );
}
