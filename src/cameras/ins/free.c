entity spawn_free_camera(
    ecs* world,
    float3 position,
    float4 rotation,
    int2 size,
    int2 screen_position)
{
    zox_instance(prefab_free_camera);
    zox_name("free_camera");
    zox_setv(e, Position3D, position);
    zox_setv(e, Rotation3D, rotation);
    zox_setv(e, ScreenDimensions, size);
    zox_setv(e, ScreenPosition, screen_position);
#ifndef zox_test_quaternion_camera
    zox_add(e, EulerOverride);
    zox_setv(e, Euler, quaternion_to_euler(rotation));
    zox_set(e, EulerLimitX, {{ -camera_limit_x, camera_limit_x }}); // adds a limit, - half pi to + half pi, 0.8 is around half that
#endif
    return e;
}
