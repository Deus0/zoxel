#define main_camera_rotation_speed 60 * 0.22f
float4 main_menu_rotation_speed;

// spawns game camera and ui camera on top
entity2 spawn_camera_player(ecs *world, entity player, byte camera_mode, float3 camera_position, float4 camera_rotation, float4 screen_to_canvas, int2 viewport_position, int2 viewport_size, int2 canvas_size) {

    main_menu_rotation_speed = quaternion_from_euler((float3) { 0, -main_camera_rotation_speed * degreesToRadians, 0 });

    float fov = get_camera_state_fov(camera_mode);
    entity e = spawn_camera(world, prefab_camera_game, camera_position, camera_rotation, camera_mode, fov, viewport_position, viewport_size, screen_to_canvas);
    zox_name("camera_game");

    zox_set(player, CameraLink, { e });
    entity e2 = spawn_camera_ui(world, prefab_camera_ui, viewport_position, canvas_size, screen_to_canvas);
    zox_set_unique_name(e2, "camera_game_ui");

    return (entity2) { e, e2 };
}
