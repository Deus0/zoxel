entity xr_camera;

void xr_spawn_camera(ecs* world, entity app) {
    // get players underneath app?
    byte camera_mode = zox_game_camera_mode;
    float fov = get_camera_state_fov(camera_mode);
    float3 camera_position = main_menu_camera_position;
    float4 camera_rotation = main_menu_camera_rotation;
    int2 screen_size = get_screen_size();
    float4 screen_to_canvas = (float4) { 1, 1, 0, 0 };
    int2 viewport_size = screen_to_canvas_size(screen_size, screen_to_canvas);
    int2 viewport_position = screen_to_canvas_position(screen_size, screen_to_canvas);
    int2 scaled_viewport_size = scale_viewport(viewport_size);
    entity player = zox_players[0];
    xr_camera = spawn_camera(
        world,
        prefab_camera_xr,
        camera_position,
        camera_rotation,
        camera_mode,
        fov,
        viewport_position,
        scaled_viewport_size,
        screen_to_canvas);
    zox_link(world, player, Camera, xr_camera);
    main_cameras[0] = xr_camera;
    zox_log("[XR] Spawned camera at [%fx%fx%f]",
        camera_position.x,
        camera_position.y,
        camera_position.z);
    // Spawn start ui
    spawn_menu_start(world, player, 0);
}
