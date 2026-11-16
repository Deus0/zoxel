entity spawn_prefab_xr_camera(ecs* world) {
    entity e = zox_prefab_from_parent(world, prefab_camera);
    zox_add(e, XrCamera);
    zox_add(e, Camera3);        // a 3D camera: billboards (the 3D menus) face it
    zox_add(e, Perspective);
    return e;
}