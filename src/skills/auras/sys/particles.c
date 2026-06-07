zox_sys2(AuraParticlesSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Activate);
    zox_sys_in(SkillActive);
    zox_sys_in(SkillRange);
    zox_sys_in(Color);
    zox_sys_out(ParticlesEmitterLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Activate, activate);
        zox_sys_i(SkillActive, active);
        zox_sys_i(SkillRange, range);
        zox_sys_i(Color, colorr);
        zox_sys_o(ParticlesEmitterLink, link);
        if (activate->value != zox_dirty_end) {
            continue;
        }
        entity user = zox_get_parent(world, e);
        if (!zox_valid(user)) {
            continue;
        }
        if (active->value) {
            // spawn particles
            if (!zox_valid(link->value)) {
                // zox_geter_value(user, Bounds3D, float3, bounds);
                // bounds.y = range->value;
                // bounds.x = 2 * range->value;
                float3 bounds = float3_single(range->value);
                link->value = spawn_particle3D_emitter(world, user, 1, bounds, colorr->value);
            }
        } else {
            // destroy them
            if (zox_valid(link->value)) {
                zox_delete(link->value);
            }
            link->value = 0;
        }
    }
} zox_sys_end(AuraParticlesSystem);
