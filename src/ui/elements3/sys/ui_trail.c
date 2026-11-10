#ifdef zox_debug_ui_trails
    float ui_trail_debug_thickness = 2.0f;
    extern entity spawn_line3(ecs*, float3, float3, float, double);
#endif

// NOTE: Assumes that target is a root transform
void trail_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(UITrail);
    zox_sys_out(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(UITrail, trail);
        zox_sys_o(Position3D, position);
        // NOTE: Deleted with its holder, as UIHolderLink is (OnDeleteTarget, Delete)
        entity holder = zox_get_link(world, e, UIHolderLink);
        if (!zox_valid(holder)) {
            continue;
        }
        float3 target_position = zox_getv(holder, Position3D);
        float3 bounds = zox_getv(holder, Bounds3D);
        float3 bounds_offset = (float3) { 0, bounds.y, 0 };
        position->value =
            float3_add(
                float3_add(
                    target_position,
                    bounds_offset
                ),
                trail->value
            );
#ifdef zox_debug_ui_trails
        spawn_line3(
            world,
            target_position,
            position3D->value,
            ui_trail_debug_thickness,
            1.0);
#endif

    }
    zox_sys_on_end();
} zoxd_system(trail_system);
