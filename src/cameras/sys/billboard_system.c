// #define zox_debug_billboard_system
#ifdef zox_debug_billboard_system
extern entity spawn_line3(ecs*, float3, float3, float, double);
#endif

zox_sys2(BillboardSystem) {
    byte dbg_log = 0;
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
        if (!zox_valid(camera) || !zox_has(camera, Rotation3D)) {
            zox_loge("No Cameras in [BillboardSystem]");
            zox_dbg_print_cameras(world);
            continue;
        }
        if (dbg_log) {
            zox_log("Closest Camera [%s]", zox_get_name(camera));
        }
        rotation->value = zox_getv(camera, Rotation3D);
#ifdef zox_debug_billboard_system
        float3 normal = quaternion_to_normal(rotation->value);
        spawn_line3(world, position->value, float3_add(position->value, normal), 2, 1);
#endif
    }
} zox_sys_end(BillboardSystem);
