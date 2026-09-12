#define main_camera_rotation_speed 60 * 0.22f
float4 main_menu_rotation_speed;

// spawns game camera and ui camera on top
entity2 spawn_player_cameras(
    ecs* world,
    entity app,
    entity player,
    byte camera_mode,
    float3 camera_position,
    float4 camera_rotation,
    float4 screen_to_canvas,
    int2 vp_position,
    int2 game_vpsize,
    int2 ui_vpsize)
{
    main_menu_rotation_speed = quaternion_from_euler((float3) { 0, -main_camera_rotation_speed * degreesToRadians, 0 });
    float fov = get_camera_state_fov(camera_mode);
    entity e = spawn_camera(
        world,
        prefab_camera_game,
        camera_position,
        camera_rotation,
        camera_mode,
        fov,
        vp_position,
        game_vpsize,
        screen_to_canvas);
    zox_set_parent(world, e, app);
    zox_link(world, app, ViewportCamera, e);
    zox_set_unique_name(e, "camera_game");
    entity e2 = spawn_camera_ui(
        world,
        prefab_camera_ui,
        vp_position,
        ui_vpsize,
        screen_to_canvas);
    // Links
    zox_link(world, player, CameraLink, e);
    zox_set_parent(world, e2, app);
    zox_link(world, app, ViewportCamera, e2);
    zox_set_unique_name(e2, "camera_game_ui");
    return (entity2) { e, e2 };
}
