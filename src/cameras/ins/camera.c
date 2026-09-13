entity spawn_camera(
    ecs *world,
    entity prefab,
    float3 position,
    float4 rotation,
    byte state,
    float fov,
    int2 screen_position,
    int2 size,
    float4 screen_to_canvas)
{
    zox_instance(prefab);
    zox_name("camera");
    zox_setv(e, Position3D, position);
    zox_setv(e, Rotation3D, rotation);
    zox_setv(e, CameraState, state);
    zox_setv(e, FieldOfView, fov);
    zox_setv(e, ScreenPosition, screen_position);
    zox_setv(e, PixelSize, size);
    zox_setv(e, ScreenToCanvas, screen_to_canvas);
    zox_setv(e, FogColor, menu_sky_color);
    return e;
}
