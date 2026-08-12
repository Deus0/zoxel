// this is for ui raycasting
void prefab_add_element_raycaster(ecs *world, const entity e) {
    zox_prefab_set(e, Raycaster, { int2_zero });
    zox_prefab_set(e, RaycasterResult, { 0 });
    zox_prefab_set(e, RaycasterTarget, { 0 });
}

// this is for world raycasting
void prefab_add_raycasts3D(ecs *world, const entity e) {
    zox_prefab_set(e, RaycastOrigin, { float3_zero });
    zox_prefab_set(e, RaycastNormal, { float3_forward });
}

void prefab_set_game_prefabs(ecs *world) {
    prefab_add_raycasts3D(world, prefab_camera_game);
    // prefab_add_element_raycaster(world, prefab_device);
    prefab_add_element_raycaster(world, prefab_zevice_pointer);
    // prefab_add_element_raycaster(world, prefab_finger);
    // prefab_add_element_raycaster(world, prefab_player);
}
