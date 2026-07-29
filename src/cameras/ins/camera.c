entity spawn_camera(ecs *world, entity prefab, float3 position, float4 rotation, byte state, float fov, int2 screen_position, int2 size, float4 screen_to_canvas) {
    zox_instance(prefab);
    zox_name("camera");
    zox_set(e, Position3D, { position });
    zox_set(e, Rotation3D, { rotation });
    zox_set(e, CameraState, { state });
    zox_set(e, FieldOfView, { fov });
    zox_set(e, ScreenPosition, { screen_position });
    zox_set(e, ScreenDimensions, { size });
    zox_set(e, ScreenToCanvas, { screen_to_canvas });
    return e;
}
