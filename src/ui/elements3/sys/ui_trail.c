#ifdef zox_debug_ui_trails
    float ui_trail_debug_thickness = 2.0f;
    extern entity spawn_line3(ecs*, float3, float3, float, double);
#endif

// NOTE: Assumes that target is a root transform
void trail_system(iter* it) {
    zox_sys_on_begin();
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
            zox_loge("CharacterUI [%s] Not Destroyed.",
                zox_getn(e));
            zox_delete(e);
            continue;
        }
        float3 target_position = zox_getv(holder->value, Position3D);
        float3 bounds = zox_getv(holder->value, Bounds3D);
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
