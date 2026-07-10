#include "mouse_raycaster_system.c"
#include "zevice_raycaster_system.c"
#include "camera_ray_system.c"
#include "ray_debug_system.c"

void zox_define_systems_raycasts(ecs* world) {
    zox_system(
        ZeviceRaycasterSystem,
        zoxp_update,
        [in] inputs.ZevicePointerPosition,
        [out] raycasts.Raycaster
    );
    #ifndef zox_debug_camera_rays
    zox_system(
        CameraRaySystem,
        zoxp_update,
        [out] raycasts.RaycastOrigin,
        [out] raycasts.RaycastNormal
    );
    #else
    zox_system_1(
        CameraRaySystem,
        zoxp_mainthread,
        [out] raycasts.RaycastOrigin,
        [out] raycasts.RaycastNormal
    );
    #endif
    #ifdef zox_debug_rays
    zox_system_1(
        RayDebugSystem,
        zoxp_mainthread,
        [in] raycasts.RaycastOrigin,
        [in] raycasts.RaycastNormal
    );
    #endif
}
