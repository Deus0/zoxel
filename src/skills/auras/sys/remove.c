// if poisoned, remove if leaving area
// I could do this per debuff instead of character...! if it's a area based debuff
// NOTE: When a character has AuraDots, we check if still in range!
zox_sys2(AuraRemoveSystem) {
    // const float damage_radius = 3.0f; // todo: grab this off skill
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        entity dots[zox_children_capacity];
        uint dots_length = zox_get_children_by_id(world, e, dots, zox_children_capacity, zox_id(AuraDot));
        for (uint j = 0; j < dots_length; j++) {
        // for (int j = dotLinks->length - 1; j >= 0; j--) {
            entity dot = dots[j];
            if (!zox_valid(dot) || !zox_has(dot, SkillLink)) {
                continue;
            }
            zox_geter_value(dot, SkillLink, entity, aura)
            zox_geter_value(dot, SpawnerLink, entity, user)
            if (!zox_valid(aura) || !zox_valid(user)) {
                // todo: remove dots when aura dies too
                continue;
            }
            zox_geter_value(aura, SkillRange, float, radius)
            zox_geter_value(aura, SkillActive, byte, active)
            byte is_still_in_aura = 0;
            if (active) { // if user exists and skill is active
                zox_geter_value(user, Position3D, float3, aura_position)
                float distance = float3_distance(position->value, aura_position);
                is_still_in_aura = distance <= radius;
            }
            if (!is_still_in_aura) {
                // zox_log(" + no longer within aura [%i]\n", j)
                zox_geter_value(dot, ParticlesEmitterLink, entity, particles);
                if (zox_valid(particles)) {
                    zox_delete(particles);
                }
                zox_delete(dot);
            }
        }

    }
} zox_sys_end(AuraRemoveSystem);
