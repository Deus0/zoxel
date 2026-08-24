entity spawn_prefab_xr_camera(ecs* world) {
    entity e = zox_prefab_from_parent(world, prefab_camera);
    zox_add(e, XrCamera);
    return e;
}