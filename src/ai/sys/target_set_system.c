zox_sys2(ThreatPositionSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(ThreatPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(ThreatPosition, attacker_position);
        entity attacker = zox_get_link(world, e, LastDamager);
        if (!zox_valid(attacker)) {
            continue;
        }
        zox_geter(attacker, Position3D, target);
        attacker_position->value = target->value;
        if (is_debug_target) {
            zox_geter_value(e, Position3D, float3, position);
            debug_linec(world, position, attacker_position->value, color_rgb_gray);
        }
    }
} zox_sys_end(ThreatPositionSystem);