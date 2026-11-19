// an attacking npc moves into its skill's reach of its threat and aims at it (RaycastAim): its turning lags a moving
//  player, its punches missed them (tst npc_attacks)
//  - melee: after a swing it steps back and to a side till near its next (AttackAt), then lunges in, a beat to dodge in
//  - ranged (a reach with a min): it stops in its reach (MoveToBuffer), facing its threat, too close it picks punch
#define npc_attack_lunge 0.5            // seconds before its next swing it comes in
#define npc_attack_step_back 2.0f       // how far it backs off

void attack_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Behaviour);
    zox_sys_out(TargetPosition);
    zox_sys_in(Position3D);
    zox_sys_in(AttackAt);
    zox_sys_in(ActiveAction);
    zox_sys_out(MoveToBuffer);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Behaviour, behaviour);
        zox_sys_o(TargetPosition, target);
        zox_sys_i(Position3D, position);
        zox_sys_i(AttackAt, attack_at);
        zox_sys_i(ActiveAction, action);
        zox_sys_o(MoveToBuffer, buffer);
        if (behaviour->value != zox_behaviour_attack) {
            if (zox_has(e, RaycastAim)) {
                zox_remove(e, RaycastAim);
                buffer->value = default_npc_follow_distance;
            }
            continue;
        }
        float3 threat;
        if (!npc_threat_position(world, e, &threat)) {
            continue;
        }
        zox_set(e, RaycastAim, { threat });
        const float2 reach = npc_skill_reach(world, e, action->value);
        if (reach.x > 0) {
            buffer->value = 0.5f * (reach.x + reach.y);
            target->value = threat;
        } else if (zox_current_time < attack_at->value - npc_attack_lunge) {
            buffer->value = default_npc_follow_distance;
            // back and to its side (each npc its own, by its id), it circles as it backs off
            float3 away = float3_subtract(position->value, threat);
            away.y = 0;
            away = float3_normalize(away);
            const float side = (e & 1) ? 1 : -1;
            const float3 step = { away.x - away.z * side, 0, away.z + away.x * side };
            target->value = float3_add(threat, float3_scale(float3_normalize(step), npc_attack_step_back));
        } else {
            buffer->value = default_npc_follow_distance;
            target->value = threat;
        }
    }
    zox_sys_on_end();
} zoxd_system(attack_system);
