// an attacking npc moves to its threat and aims its ray at it (RaycastAim): its turning lags a moving player, its
//  punches missed them (tst npc_attacks)
//  - after a swing it steps back and to a side till near its next (AttackAt), then lunges in: a beat to dodge in
#define npc_attack_lunge 0.5            // seconds before its next swing it comes in
#define npc_attack_step_back 2.0f       // how far it backs off

void attack_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Behaviour);
    zox_sys_in(ThreatPosition);
    zox_sys_out(TargetPosition);
    zox_sys_in(Position3D);
    zox_sys_in(AttackAt);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Behaviour, behaviour);
        zox_sys_i(ThreatPosition, threat);
        zox_sys_o(TargetPosition, target);
        zox_sys_i(Position3D, position);
        zox_sys_i(AttackAt, attack_at);
        if (behaviour->value != zox_behaviour_attack) {
            if (zox_has(e, RaycastAim)) {
                zox_remove(e, RaycastAim);
            }
            continue;
        }
        zox_set(e, RaycastAim, { threat->value });
        if (zox_current_time < attack_at->value - npc_attack_lunge) {
            // back and to its side (each npc its own, by its id), it circles as it backs off
            float3 away = float3_subtract(position->value, threat->value);
            away.y = 0;
            away = float3_normalize(away);
            const float side = (e & 1) ? 1 : -1;
            const float3 step = { away.x - away.z * side, 0, away.z + away.x * side };
            target->value = float3_add(threat->value, float3_scale(float3_normalize(step), npc_attack_step_back));
        } else {
            target->value = threat->value;
        }
    }
    zox_sys_on_end();
} zoxd_system(attack_system);
