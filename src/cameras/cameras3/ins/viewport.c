// Spawns a camera drawing straight to a part of the app's window
//  - screen_to_canvas: its part of the window (size xy, position zw, 0 to 1), see viewport_grid_cell
//  - linked to the app's ViewportCamera, so viewport_resize_system keeps it sized with the window
//  - delete it to remove the view (the link is non fragmenting, the app forgets it)
entity spawn_viewport_camera(
    ecs* world,
    entity app,
    float3 position,
    float4 rotation,
    float fov,
    float4 screen_to_canvas)
{
    const int2 window_size = zox_valid(app) && zox_has(app, WindowSize) ?
        zox_getv(app, WindowSize) :
        int2_zero;
    const int2 size = screen_to_canvas_size(window_size, screen_to_canvas);
    const int2 screen_position = screen_to_canvas_position(window_size, screen_to_canvas);
    entity e = spawn_camera(
        world,
        prefab_camera_viewport,
        position,
        rotation,
        zox_camera_state_idle,
        fov,
        screen_position,
        size,
        screen_to_canvas);
    zox_set_unique_name(e, "camera_viewport");
    if (zox_valid(app)) {
        zox_link(world, app, ViewportCamera, e);
    }
    return e;
}
