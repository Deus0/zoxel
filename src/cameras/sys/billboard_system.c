// #define zox_debug_billboard_system

#ifdef zox_debug_billboard_system
extern entity spawn_line3(ecs*, float3, float3, float, double);
#endif

zox_sys2(BillboardSystem) {
    if (!main_cameras_count) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_out(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_o(Rotation3D, rotation);

        entity camera = find_closest_camera(world, position->value);
        if (!camera || !zox_has(camera, Rotation3D)) {
            continue;
        }

        zox_geter_value(camera, Rotation3D, float4, camera_rotation);
        rotation->value = camera_rotation;

#ifdef zox_debug_billboard_system
        float3 normal = quaternion_to_normal(rotation->value);
        spawn_line3(world, position->value, float3_add(position->value, normal), 2, 1);
#endif
    }
} zox_sys_end(BillboardSystem);
