#ifndef zoxm_raycasts
#define zoxm_raycasts

// #define zox_debug_camera_rays // used to verify raycast line is straight
// todo later: fix rays for coop, viewport distortion effects rays
// #define zox_debug_rays
#include "dat/ray.c"
// 2D
zoxc_byte(RaycasterResult);
zoxc_entity(RaycasterTarget);       //! A target entity for the Raycaster
zoxc_int2(Raycaster);               //! Contains the raycast mouse position
// 3D
zoxc_float3(RaycastOrigin);
zoxc_float3(RaycastNormal);
#include "fun/camera.c"
#include "fun/prefab.c"
#include "sys/_.c"

zox_begin_module(Raycasts)
    add_hook_spawn_prefabs(prefab_set_game_prefabs);
    zoxd_int2(Raycaster);
    zoxd_byte(RaycasterResult);
    zoxd_entity(RaycasterTarget);
    zoxd_float3(RaycastOrigin);
    zoxd_float3(RaycastNormal);
    zox_system(
        ZeviceRaycasterSystem,
        zoxp_inputs_update,
        [in] inputs.ZevicePointerPosition,
        [out] raycasts.Raycaster
    );
#ifndef zox_debug_camera_rays
    zox_system(
        CameraRaySystem,
        zoxp_inputs_update,
        [out] raycasts.RaycastOrigin,
        [out] raycasts.RaycastNormal
    );
#else
    zox_system_1(
        CameraRaySystem, zoxp_mainthread,
        [out] raycasts.RaycastOrigin,
        [out] raycasts.RaycastNormal
    );
#endif
#ifdef zox_debug_rays
    zox_system_1(
        RayDebugSystem, zoxp_mainthread,
        [in] raycasts.RaycastOrigin,
        [in] raycasts.RaycastNormal
    );
#endif
zox_end_module(Raycasts)

#endif
