entity spawn_camera_ui(
    ecs *world,
    entity prefab,
    int2 position,
    int2 dimensions,
    float4 screen_to_canvas)
{
    zox_instance(prefab);
    zox_name("ui_camera");
    zox_setv(e, ScreenPosition, position);
    zox_setv(e, PixelSize, dimensions);
    zox_setv(e, ScreenToCanvas, screen_to_canvas);
    return e;
}
