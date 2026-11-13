void threat_position_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(ThreatPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(ThreatPosition, attacker_position);
        entity attacker = zox_get_link(world, e, AttackerLink);
        if (!zox_valid(attacker)) {
            continue;
        }
        const Position3D* target = zox_get(attacker, Position3D);
        attacker_position->value = target->value;
        if (is_debug_target) {
            float3 position = zox_getv(e, Position3D);
            debug_linec(world, position, attacker_position->value, color_rgb_gray);
        }
    }
    zox_sys_on_end();
} zoxd_system(threat_position_system);
