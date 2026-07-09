// if poisoned, remove if leaving area
// I could do this per debuff instead of character...! if it's a area based debuff
// NOTE: When a character has AuraDots, we check if still in range!
zox_sys2(AuraDotRemoveSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkillLink);
    zox_sys_in(SpawnerLink);
    zox_sys_in(ParticlesEmitterLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SkillLink, aura);
        zox_sys_i(SpawnerLink, user);
        zox_sys_i(ParticlesEmitterLink, particles);
        // if user exists and skill is active
        if (zox_valid(user->value) && zox_valid(aura->value)) {
            if (zox_getv(aura->value, SkillActive)) {
                float3 aura_position = zox_getv(user->value, Position3D);
                entity cursed = zox_get_parent(world, e);
                float3 cursed_position = zox_getv(cursed, Position3D);
                float aura_radius = zox_getv(aura->value, SkillRange);
                float distance = float3_distance(cursed_position, aura_position);
                if (distance <= aura_radius) {
                    continue;
                }
            }
        }
        if (dbg_log) {
            zox_log("Dot No Longer within aura [%s]", zox_get_name(e));
        }
        if (zox_valid(particles->value)) {
            zox_delete(particles->value);
        }
        zox_delete(e);
    }
} zox_sys_end(AuraDotRemoveSystem);
