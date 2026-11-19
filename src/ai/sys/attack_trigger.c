// an attacking npc picks its skill for its threat's distance (npc_choose_skill: punch close, shoot further) and uses
//  it when its threat is in that skill's reach and in front of it, its beat uneven (AttackAt): a swing, a step back,
//  a lunge in (attack_system), not a swing at the air every cooldown; its beat on its clock (ClockTime: paused it waits)
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
    zox_sys_out(AttackAt);
    zox_sys_in(ClockTime);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Behaviour, behaviour);
        zox_sys_o(ActiveAction, aaction);
        zox_sys_o(ActiveActionDirty, dirty);
        zox_sys_o(TriggerActionA, trigger);
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_o(AttackAt, attack_at);
        zox_sys_i(ClockTime, now);
        if (behaviour->value != zox_behaviour_attack || trigger->value || dirty->value) {
            continue;
        }
        float3 threat;
        if (!npc_threat_position(world, e, &threat)) {
            continue;
        }
        float3 to_threat = float3_subtract(threat, position->value);
        to_threat.y = 0;
        const float distance = float3_length(to_threat);
        const entity skill = npc_choose_skill(world, e, aaction->value, distance);
        if (skill != aaction->value) {
            aaction->value = skill;
            dirty->value = 1;
            continue;
        }
        if (!zox_valid(skill) || now->value < attack_at->value) {
            continue;
        }
        const float2 reach = npc_skill_reach(world, e, skill);
        const float3 forward = quaternion_to_normal(rotation->value);
        if (distance < reach.x || distance > reach.y ||
            (distance > 0.01f && float3_dot(forward, float3_scale(to_threat, 1 / distance)) < npc_attack_facing)) {
            continue;
        }
        trigger->value = zox_dirty_trigger;
        attack_at->value = now->value + randf_range(npc_attack_beat_min, npc_attack_beat_max);
    }
    zox_sys_on_end();
} zoxd_system(attack_trigger_system);
