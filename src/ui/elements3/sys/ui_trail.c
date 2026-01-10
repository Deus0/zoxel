#ifdef zox_debug_ui_trails
    float ui_trail_debug_thickness = 2.0f;
    extern entity spawn_line3D(ecs*, float3, float3, float, double);
#endif

zox_sys2(UITrailSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(UIHolderLink);
    zox_sys_in(UITrail);
    zox_sys_out(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(UIHolderLink, holder);
        zox_sys_i(UITrail, trail);
        zox_sys_o(Position3D, position);

        if (!zox_valid(holder->value)) {
            zox_log_error("Character UI wasn't destroyed: %s", zox_get_name(e));
            zox_delete(e);
            continue;
        }

        position->value = trail->value;
        zox_geter_value(holder->value, Position3D, float3, target_position);
        zox_geter_value(holder->value, Bounds3D, float3, bounds);
        float3_add_float3_p(&position->value, target_position);
        float3_add_float3_p(&position->value, (float3) { 0, bounds.y, 0 });
        if (zox_has(e, Children)) {
            zox_geter(e, Children, children);
            for (int j = 0; j < children->length; j++) {
                const entity child = children->value[j];
                zox_geter(child, LocalPosition3D, child_local_position3D);
                zox_muter(child, Position3D, child_position3D);
                set_position_from_parents(world, e, &child_position3D->value, child_local_position3D->value);
            }
        }

#ifdef zox_debug_ui_trails
        spawn_line3D(world, target_position, position3D->value, ui_trail_debug_thickness, 1.0);
#endif

    }
} zox_sys_end(UITrailSystem);