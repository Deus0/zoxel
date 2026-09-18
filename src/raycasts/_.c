// TODO: Make this disable/enable system instead
byte zox_dbg_rays = 0;

//zoxc_byte(RaycasterResult);
//zoxc_entity(EntityTarget);       //! A target entity for the Raycaster
zoxc_int2(Raycaster);               //! Contains the raycast mouse position
zoxc_float3(RaycastOrigin);
zoxc_float3(RaycastNormal);
#include "fun/_.c"
#include "sys/_.c"

void import_raycasts(ecs* world) {
    zox_module(raycasts);
    zoxd_int2(Raycaster);
    //zoxd_byte(RaycasterResult);
    //zoxd_entity(EntityTarget);
    zoxd_float3(RaycastOrigin);
    zoxd_float3(RaycastNormal);
    zox_systems_raycasts(world);
    add_hook_spawn_prefabs(prefab_set_game_prefabs);
}
