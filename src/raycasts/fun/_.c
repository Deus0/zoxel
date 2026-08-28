// this is for ui raycasting
void prefab_add_element_raycaster(ecs* world, entity e) {
    zox_setv(e, Raycaster, int2_zero);
    //zox_setv(e, RaycasterResult, 0);
    zox_setv(e, EntityTarget, 0);
}

// this is for world raycasting
void prefab_add_raycasts3D(ecs* world, entity e) {
    zox_setv(e, RaycastOrigin, float3_zero);
    zox_setv(e, RaycastNormal, float3_forward);
}

void prefab_set_game_prefabs(ecs* world) {
    if (!prefab_camera_game) {
        zox_loge("Invalid [prefab_camera_game]");
        return;
    }
    if (!prefab_zevice_pointer || !prefab_gamepad) {
        zox_loge("Invalid [prefab_zevice_pointer] or [prefab_gamepad]");
        return;
    }
    prefab_add_raycasts3D(world, prefab_camera_game);
    prefab_add_element_raycaster(world, prefab_zevice_pointer);
    zox_prefab_set(prefab_gamepad, EntityTarget, { 0 });
}
