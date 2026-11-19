// an attacking npc swings when its threat is in reach and in front of it, its beat uneven (AttackAt): a swing, a
//  step back, a lunge in (attack_system), not a swing at the air every cooldown
#define npc_attack_facing 0.5f          // its threat within 60 degrees of its facing
#define npc_attack_beat_min 0.8         // seconds from a swing to the next, at least
#define npc_attack_beat_max 1.6

void attack_trigger_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Behaviour);
    zox_sys_out(ActiveAction);
    zox_sys_out(ActiveActionDirty);
    zox_sys_out(TriggerActionA);
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    zox_sys_in(ThreatPosition);
    zox_sys_out(AttackAt);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Behaviour, behaviour);
        zox_sys_o(ActiveAction, aaction);
        zox_sys_o(ActiveActionDirty, dirty);
        zox_sys_o(TriggerActionA, trigger);
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_i(ThreatPosition, threat);
        zox_sys_o(AttackAt, attack_at);
        if (behaviour->value != zox_behaviour_attack || trigger->value || dirty->value) {
            continue;
        }
        if (!zox_valid(aaction->value)) {
            aaction->value = zox_get_child_by_id(world, e, zox_id(Skill));
            dirty->value = 1;
            continue;
        }
        if (zox_current_time < attack_at->value) {
            continue;
        }
        // its reach: the skill's range to the threat's side (a body's half width past its centre)
        const float reach = (zox_has(aaction->value, SkillRange) ? zox_getv(aaction->value, SkillRange) : 1.5f) + 0.25f;
        float3 to_threat = float3_subtract(threat->value, position->value);
        to_threat.y = 0;
        const float distance = float3_length(to_threat);
        const float3 forward = quaternion_to_normal(rotation->value);
        if (distance > reach || (distance > 0.01f && float3_dot(forward, float3_scale(to_threat, 1 / distance)) < npc_attack_facing)) {
            continue;
        }
        trigger->value = zox_dirty_trigger;
        attack_at->value = zox_current_time + randf_range(npc_attack_beat_min, npc_attack_beat_max);
    }
    zox_sys_on_end();
} zoxd_system(attack_trigger_system);
