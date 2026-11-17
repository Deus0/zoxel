// The XR camera: the game camera's parts (streaming, raycasts, biomes), rendered per eye
//  - not the game camera's prefab: no render texture or its post processing (blur, vignette), its eyes draw
//      straight to a swapchain or the window (inherited components can't be taken off a child prefab)
entity spawn_prefab_xr_camera(ecs* world) {
    entity e = zox_prefab_from_parent(world, prefab_camera);
    zox_prefab_name("camera_xr");
    zox_add(e, XrCamera);
    zox_add(e, Camera3);        // a 3D camera: billboards (the 3D menus) face it
    zox_add(e, Perspective);
    prefab_add_streamer(world, e);      // terrain streams around it (game start places it at the spawn)
    prefab_add_raycasts3D(world, e);    // the player's voxel raycasts start from it
    zox_add(e, BiomeLinker);
    zox_setv(e, FieldOfViewAngles, xr_cull_angles_default);    // its eyes' views, set each frame (fun/cull.c)
    return e;
}
