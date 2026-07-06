// #define zox_debug_billboard_system
#ifdef zox_debug_billboard_system
extern entity spawn_line3(ecs*, float3, float3, float, double);
#endif

// NOTE: Makes UIs look at the cameras
zox_sys2(BillboardSystem) {
    byte dbg_log = 0;
    zox_sys_query();
    zox_sys_world();
    // cache camera positions first
    float3_array_d* camera_postiions = create_float3_array_d(1);
    float4_array_d* camera_rotations = create_float4_array_d(1);
    zox_sys_query_begin();
    while (zox_sys_query_loop()) {
        zox_sys_begin_2();
        zox_sys_in_2(Position3D);
        zox_sys_in_2(Rotation3D);
        for (int j = 0; j < it2.count; j++) {
            // zox_sys_e_2();
            zox_sys_i_2(Position3D, camera_position);
            zox_sys_i_2(Rotation3D, camera_rotation);
            float3_array_d_add(camera_postiions, camera_position->value);
            float4_array_d_add(camera_rotations, camera_rotation->value);
        }
    }
    zox_sys_query_end();
    if (!camera_postiions->size) {
        zox_loge("No Cameras in [BillboardSystem]");
        return;
    }
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    zox_sys_in(Position3D);
    zox_sys_out(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(Position3D, position);
        zox_sys_o(Rotation3D, rotation);
        if (disabled->value) {
            continue;
        }
        // entity closest_camera = 0;
        // byte found_camera = 0;
        float4 closest_rotation = quaternion_identity;
        float closest_distance = -1;
        uint total_cameras = 0;
        for (size_t j = 0; j < camera_postiions->size; j++) {
            float3 camera_position = camera_postiions->data[j];
            float4 camera_rotation = camera_rotations->data[j];
            float distance = float3_distance_squared(position->value, camera_position);
            if (closest_distance == -1 || distance < closest_distance) {
                closest_distance = distance;
                closest_rotation = camera_rotation;
                // found_camera = 1;
                // closest_camera = e2;
            }
        }
        /*zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2();
            zox_sys_in_2(Position3D);
            zox_sys_in_2(Rotation3D);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_e_2();
                zox_sys_i_2(Position3D, camera_position);
                zox_sys_i_2(Rotation3D, camera_rotation);
                float distance = float3_distance_squared(position->value, camera_position->value);
                if (distance < closest_distance) {
                    closest_distance = distance;
                    closest_camera = e2;
                    closest_rotation = camera_rotation->value;
                }
            }
            total_cameras += it2.count;
        }
        zox_sys_query_end();*/
        rotation->value = closest_rotation;
        zox_sys_increment();
        if (dbg_log) {
            zox_sys_e();
            zox_log("[%s]'s Closest Camera of total: [%i]", zox_get_name(e), total_cameras);
        }
#ifdef zox_debug_billboard_system
        float3 normal = quaternion_to_normal(rotation->value);
        spawn_line3(world, position->value, float3_add(position->value, normal), 2, 1);
#endif
    }
    dispose_float3_array_d(camera_postiions);
    dispose_float4_array_d(camera_rotations);
} zox_sys_end(BillboardSystem);
