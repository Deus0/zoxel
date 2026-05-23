// #define zox_debug_aoe_damage_system
#ifdef zox_debug_aoe_damage_system
    extern entity spawn_line3(ecs *world, float3 pointA, float3 pointB, float thickness, double life_time);
#endif

zox_sys2(AuraSystem) {
    byte dbg_log = 1;
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkillActive);
    zox_sys_in(SkillDamage);
    zox_sys_in(SkillRange);
    zox_sys_in(Color);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SkillActive, skillActive);
        zox_sys_i(SkillDamage, skillDamage);
        zox_sys_i(SkillRange, skillRange);
        zox_sys_i(Color, colorr);
        entity user = zox_get_parent(world, e);
        if (!zox_valid(user) || !skillActive->value || !skillDamage->value || !skillRange->value) {
            continue;
        }
        zox_geter_value(user, Position3D, float3, position3)
        // todo: Get Chunk' Characters instead, this could potentially go through tens of thousands..
        // get nearby characters using distance formula
        // make this spherecast
        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2();
            zox_sys_in_2(Dead);
            zox_sys_in_2(Position3D);
            zox_sys_out_2(DotLinks);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_e_2();
                zox_sys_i_2(Position3D, position3D2)
                zox_sys_i_2(Dead, dead);
                zox_sys_o_2(DotLinks, dotLinks);
                if (user == e2 || dead->value) {
                    continue;
                }
                float distance = float3_distance(position3, position3D2->value);
                entity poisoned_entity = 0;
                // Checks if dot was already added to player!
                // get poison, that  was initiated by this aura user
                for (int k = 0; k < dotLinks->length; k++) {
                    entity dot = dotLinks->value[k];
                    if (!zox_has(dot, SkillLink)) {
                        continue;
                    }
                    zox_geter_value(dot, SkillLink, entity, skill_spawner);
                    if (skill_spawner == e) {
                        poisoned_entity = dot;
                        break;
                    }
                }
                if (poisoned_entity) {
                    continue;
                }
                // makes sure to check the debuff is linked to same character
                // makes it so t two players can damage a character at once
                if (distance <= skillRange->value) {
                    entity e3 = spawn_poison(world, e2, prefab_poison, user, e, skillDamage->value);
                    if (dbg_log) {
                        zox_log("Added new dot [%s] total dots [%i]", zox_get_name(e3), dotLinks->length);
                    }
                    add_to_DotLinks(dotLinks, e3);
                    // spawn particle system
                    float3 bounds = zox_get_value(e2, Bounds3D);
                    entity p = spawn_particle3D_emitter(world, e2, 4, float3_scale(bounds, 2), colorr->value);
                    zox_set(p, SkillLink, { e });
                    zox_set(e3, ParticlesEmitterLink, { p });
#ifdef zox_debug_aoe_damage_system
                    spawn_line3(world, position3, position3D2->value, 0.5f, 0.1);
#endif
                }
            }
        }
        zox_sys_query_end();
    }
} zox_sys_end(AuraSystem);
