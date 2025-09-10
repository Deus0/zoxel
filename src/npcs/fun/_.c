void set_character3_npc(ecs* world, entity c, byte npc) {
    if (npc) {
        zox_add_tag(c, Npc);
        zox_add_tag(c, Jumper);
        zox_prefab_set(c, Behaviour, { zox_behaviour_idle });
        zox_prefab_set(c, DefaultBehaviour, { zox_behaviour_wander });
        zox_prefab_set(c, MoveForwards, { 0 });
        zox_prefab_set(c, RotateTowards, { 0 });
        zox_prefab_set(c, MoveSpeed, { 4 });
        zox_prefab_set(c, TargetPosition, { float3_zero });
        zox_prefab_set(c, ThreatPosition, { float3_zero });
        zox_prefab_set(c, WanderDirection, { float3_zero });
    } else {
        zox_add_tag(c, Npc);
        zox_add_tag(c, Jumper);
        zox_remove(c, Behaviour);
        zox_remove(c, DefaultBehaviour);
        zox_remove(c, MoveForwards);
        zox_remove(c, RotateTowards);
        zox_remove(c, MoveSpeed);
        zox_remove(c, TargetPosition);
        zox_remove(c, ThreatPosition);
        zox_remove(c, WanderDirection);
    }
}

int count_entities_npc(ecs_world_t *world) {
    return zox_count_types(Npc)
}
