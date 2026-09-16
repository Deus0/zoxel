// #define zox_debug_billboard_system
#ifdef zox_debug_billboard_system
extern entity spawn_line3_alpha(ecs*, float3, float3, float, double, color);
#endif

// NOTE: Makes UIs look at the cameras
void billboard_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
#ifdef zox_debug_billboard_system
    color dbg_color = color_white;
#endif
    zox_sys_world();
    zox_sys_query();
    zox_sys_query_begin();
    uint cameras_count = 0;
    while (zox_sys_query_loop()) {
        cameras_count += it2.count;
    }
    entity cameras[cameras_count];
    float3 camera_positions[cameras_count];
    float4 camera_rotations[cameras_count];
    // cache camera positions first
    // entity_array_d* cameras = create_entity_array_d(1);
    // float3_array_d* camera_positions = create_float3_array_d(1);
    // float4_array_d* camera_rotations = create_float4_array_d(1);
    // zox_sys_query_begin();
    int camera_count = 0;
    it2 = ecs_query_iter(world, query);
    while (zox_sys_query_loop()) {
        zox_sys_begin_2();
        zox_sys_in_2(Position3D);
        zox_sys_in_2(Rotation3D);
        for (int j = 0; j < it2.count; j++) {
            zox_sys_e_2();
            zox_sys_i_2(Position3D, camera_position);
            zox_sys_i_2(Rotation3D, camera_rotation);
            /*entity_array_d_add(cameras, e2);
            float3_array_d_add(camera_positions, camera_position->value);
            float4_array_d_add(camera_rotations, camera_rotation->value);*/
            cameras[camera_count] = e2;
            camera_positions[camera_count] = camera_position->value;
            camera_rotations[camera_count] = camera_rotation->value;
            camera_count++;
        }
    }
    zox_sys_query_end();
    if (!cameras_count) {
        zox_loge("No Cameras in [billboard_system]");
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
        float4 closest_rotation = quaternion_identity;
        float closest_distance = -1;
        entity closest_camera = 0;
        for (size_t j = 0; j < cameras_count; j++) {
            float3 camera_position = camera_positions[j];
            float4 camera_rotation = camera_rotations[j];
            float distance = float3_distance_squared(position->value, camera_position);
            if (closest_distance == -1 || distance < closest_distance) {
                closest_distance = distance;
                closest_rotation = camera_rotation;
                closest_camera = cameras[j];
            }
        }
        rotation->value = closest_rotation;
        zox_sys_increment();
        if (dbg_log) {
            zox_sys_e();
            zox_log("[%s]'s Closest Camera [%s] of total: [%i]",
                zox_get_name(e),
                zox_getn(closest_camera),
                cameras_count);
        }
#ifdef zox_debug_billboard_system
        float3 normal = quaternion_to_normal(rotation->value);
        spawn_line3_alpha(
            world,
            position->value,
            float3_add(position->value, normal),
            2,
            1,
            dbg_color);
#endif
    }
    //dispose_entity_array_d(cameras);
    //dispose_float3_array_d(camera_positions);
    //dispose_float4_array_d(camera_rotations);
    zox_sys_on_end();
} zoxd_system(billboard_system);
