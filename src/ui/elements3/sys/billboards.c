void set_rotation_recursive(ecs* world, entity e, float4 rotation) {
    if (!zox_valid(e)) {
        return;
    }
    if (zox_has(e, Rotation3D)) {
        zox_setm(e, Rotation3D, rotation);
    }
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            set_rotation_recursive(world, e2, rotation);
        }
    }
}

// NOTE: Makes UIs look at the cameras
void billboard_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
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
            float distance = float3_distance_squared(
                position->value,
                camera_position);
            if (closest_distance == -1 || distance < closest_distance) {
                closest_distance = distance;
                closest_rotation = camera_rotation;
                closest_camera = cameras[j];
            }
        }
        /*float4 billboard_rotation =
            quaternion_rotate(
                closest_rotation,
                euler_to_quaternion((float3) {
                    0,
                    degrees_to_radians * 180,
                    0
                }));*/
        rotation->value = closest_rotation;
        zox_sys_e();
        // set_rotation_recursive(world, e, closest_rotation);

        float3 world_scale = zox_has(e, Scale3) ?
            zox_getv(e, Scale3) :
                (zox_has(e, Scale1) ?
                    float3_single(zox_getv(e, Scale1)) :
                    float3_one);
        set_position_rotation_scale_recursive(
            world,
            e,
            zox_getv(e, Position3D),
            rotation->value,
            world_scale);
        zox_sys_increment();
        if (dbg_log) {
            zox_sys_e();
            zox_log("[%s]'s Closest Camera [%s] of total: [%i]",
                zox_get_name(e),
                zox_getn(closest_camera),
                cameras_count);
        }
    }
    zox_sys_on_end();
} zoxd_system(billboard_system);
