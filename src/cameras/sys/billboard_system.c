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
        zox_sys_e();
        zox_sys_i(Position3D, position3D);
        zox_sys_o(Rotation3D, rotation3D);

        entity camera = find_closest_camera(world, position3D->value);

        if (!camera || !zox_has(camera, Rotation3D)) {
            continue;
        }

        float4 target_rotation = zox_get_value(camera, Rotation3D)
        rotation3D->value = target_rotation;
        if (zox_has(e, Children)) {
            zox_geter(e, Children, children);

            for (int j = 0; j < children->length; j++) {
                entity child = children->value[j];

                if (!zox_has(child, LocalRotation3D)) {
                    // zox_log("! billboard child [%i:%s] doesn't have [LocalRotation3D]\n", j, zox_get_name(child))
                    continue;
                }

                zox_geter_value(child, LocalRotation3D, float4, child_local_rotation3D);

                zox_muter(child, Rotation3D, child_rotation);

                set_rotation_from_parents(world, e, &child_rotation->value, child_local_rotation3D);
            }
        }
#ifdef zox_debug_billboard_system
        spawn_line3(world, position3D->value, float3_add(position3D->value, normal), 2, 1);
#endif
    }
} zox_sys_end(BillboardSystem);
