zox_sys2(AuraParticlesSystem) {
    byte aura_spawn_rate = 16;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Activate);
    zox_sys_in(SkillActive);
    zox_sys_in(SkillRange);
    zox_sys_in(Color);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Activate, activate);
        zox_sys_i(SkillActive, active);
        zox_sys_i(SkillRange, range);
        zox_sys_i(Color, colorr);
        if (activate->value != zox_dirty_end) {
            continue;
        }
        entity user = zox_get_parent(world, e);
        if (!zox_valid(user)) {
            continue;
        }
        entity particles = zox_get_link(world, e, ParticlesEmitter);
        if (active->value) {
            // spawn particles
            if (!zox_valid(particles)) {
                // zox_geter_value(user, Bounds3D, float3, bounds);
                // bounds.y = range->value;
                // bounds.x = 2 * range->value;
                float3 bounds = float3_single(range->value);
                particles = spawn_particle3D_emitter(world, user, aura_spawn_rate, bounds, colorr->value);
            }
        } else {
            // destroy them
            if (zox_valid(particles)) {
                zox_delete(particles);
            }
            particles = 0;
        }
    }
} zox_sys_end(AuraParticlesSystem);
