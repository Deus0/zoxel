// #define zox_debug_billboard_system
#ifdef zox_debug_billboard_system
extern entity spawn_line3(ecs*, float3, float3, float, double);
#endif
entity find_closest_camera(ecs *world, float3 position) {
    entity output = 0;
    float closest_distance = 100000;
    for (int j = 0; j < main_cameras_count; j++) {
        entity camera = main_cameras[j];
        if (!zox_valid(camera)) {
            continue;
        }
        float3 camera_position = zox_get_value(camera, Position3D)
        float distance = float3_distance(position, camera_position);
        if (distance < closest_distance) {
            closest_distance = distance;
            output = camera;
        }
    }
    return output;
}

// finds closest camera to a position
void zox_dbg_print_cameras(ecs* world) {
    zox_log("Cameras [%i]", main_cameras_count);
    for (byte i = 0; i < main_cameras_count; i++) {
        entity camera = main_cameras[i];
        if (!zox_valid(camera)) {
            zox_log("   - [%i]: Invalid", i);
        } else {
            zox_log("   - [%i]: %s", i, zox_get_name(camera));
        }
    }
}

zox_sys2(BillboardSystem) {
    byte dbg_log = 0;
    if (!main_cameras_count) {
        return;
    }
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_out(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_o(Rotation3D, rotation);
        entity closest_camera = 0;
        float closest_distance = 100000;
        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2();
            zox_sys_in_2(Position3D);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_e_2();
                zox_sys_i_2(Position3D, camera_position);
                float distance = float3_distance(position->value, camera_position->value);
                if (distance < closest_distance) {
                    closest_distance = distance;
                    closest_camera = e2;
                }
            }
        }
        zox_sys_query_end();
        if (!closest_camera) {
            zox_loge("No Cameras in [BillboardSystem]");
            continue;
        }
        // entity camera = find_closest_camera(world, position->value);
        /*if (!zox_valid(closest_camera) || !zox_has(closest_camera, Rotation3D)) {
            zox_loge("No Cameras in [BillboardSystem]");
            zox_dbg_print_cameras(world);
            continue;
        }*/
        if (dbg_log) {
            zox_log("Closest Camera [%s]", zox_get_name(closest_camera));
        }
        rotation->value = zox_getv(closest_camera, Rotation3D);
#ifdef zox_debug_billboard_system
        float3 normal = quaternion_to_normal(rotation->value);
        spawn_line3(world, position->value, float3_add(position->value, normal), 2, 1);
#endif
    }
} zox_sys_end(BillboardSystem);
