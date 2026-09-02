// #define zox_debug_aoe_damage_system
#ifdef zox_debug_aoe_damage_system
extern entity spawn_line3(ecs*, float3, float3, float, double);
#endif

// NOTE: This adds Dots to nearby characters!
zox_sys2(AuraDotSystem) {
    byte dbg_log = 0;
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkillActive);
    zox_sys_in(SkillDamage);
    zox_sys_in(SkillRange);
    zox_sys_in(Color);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SkillActive, active);
        zox_sys_i(SkillDamage, damage);
        zox_sys_i(SkillRange, range);
        zox_sys_i(Color, colorr);
        if (!active->value || !damage->value || !range->value) {
            continue;
        }
        entity user = zox_get_parent(world, e);
        if (!zox_valid(user)) {
            continue;
        }
        float3 position = zox_getv(user, Position3D);
        // todo: Get Chunk' Characters instead, this could potentially go through tens of thousands..
        // get nearby characters using distance formula
        // make this spherecast
        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2();
            zox_sys_in_2(Position3D);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_e_2();
                zox_sys_i_2(Position3D, position2);
                if (user == e2) {
                    continue;
                }
                float distance = float3_distance(
                    position,
                    position2->value);
                // NOTE: Checks if dot was already added to character!
                entity dots[zox_children_capacity];
                uint dots_length = zox_get_children_by_id(
                    world,
                    e2,
                    dots,
                    zox_children_capacity,
                    zox_id(Dot));
                byte was_poisoned = 0;
                for (uint k = 0; k < dots_length; k++) {
                    entity dot = dots[k];
                    if (!zox_has(dot, SkillLink)) {
                        continue;
                    }
                    entity spawner = zox_getv(dot, SkillLink);
                    if (spawner == e) {
                        was_poisoned = 1;
                        break;
                    }
                }
                if (was_poisoned) {
                    continue;
                }
                // makes sure to check the debuff is linked to same character
                // makes it so t two players can damage a character at once
                if (distance <= range->value) {
                    entity e3 = spawn_poison(
                        world,
                        e2,
                        prefab_poison,
                        user,
                        e,
                        damage->value);
                    zox_add(e3, AuraDot);
                    zox_set_parent(world, e3, e2);
                    if (dbg_log) {
                        zox_log("Added new dot [%s]", zox_get_name(e3));
                    }
                    // spawn particle system
                    float3 bounds = zox_getv(e2, Bounds3D);
                    entity particles = spawn_particle3D_emitter(
                        world,
                        e2,
                        4,
                        float3_scale(bounds, 2),
                        colorr->value);
                    zox_setv(particles, SkillLink, e);
                    zox_setv(e3, ParticlesEmitterLink, particles);
#ifdef zox_debug_aoe_damage_system
                    spawn_line3(
                        world,
                        position3,
                        position3D2->value,
                        0.5f,
                        0.1);
#endif
                }
            }
        }
        zox_sys_query_end();
    }
} zox_sys_end(AuraDotSystem);
