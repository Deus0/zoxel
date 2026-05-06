#ifdef zox_debug_ui_trails
    float ui_trail_debug_thickness = 2.0f;
    extern entity spawn_line3(ecs*, float3, float3, float, double);
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

        zox_geter_value(holder->value, Position3D, float3, target_position);
        zox_geter_value(holder->value, Bounds3D, float3, bounds);

        position->value = trail->value;

        float3_add_float3_p(&position->value, target_position);
        float3_add_float3_p(&position->value, (float3) { 0, bounds.y, 0 });

#ifdef zox_debug_ui_trails
        spawn_line3(world, target_position, position3D->value, ui_trail_debug_thickness, 1.0);
#endif

    }
} zox_sys_end(UITrailSystem);
