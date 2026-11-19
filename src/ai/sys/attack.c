// an attacking npc moves to its threat and aims its ray at it (RaycastAim): its turning lags a moving player, its
//  punches missed them (tst npc_attacks)
void attack_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Behaviour);
    zox_sys_in(ThreatPosition);
    zox_sys_out(TargetPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Behaviour, behaviour);
        zox_sys_i(ThreatPosition, threat);
        zox_sys_o(TargetPosition, target);
        if (behaviour->value != zox_behaviour_attack) {
            if (zox_has(e, RaycastAim)) {
                zox_remove(e, RaycastAim);
            }
            continue;
        }
        target->value = threat->value;
        zox_set(e, RaycastAim, { threat->value });
    }
    zox_sys_on_end();
} zoxd_system(attack_system);
