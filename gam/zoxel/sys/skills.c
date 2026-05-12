zox_sys2(SkillsRealmSpawnSystem) {
    float base_death_aura_damage = 1;
    float base_death_aura_range = 3;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(StatLinks);
    zox_sys_out(SkillLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(StatLinks, stats);
        zox_sys_o(SkillLinks, skills);
        if (state->value != zox_generate_realm_skills) {
            continue;
        }
        // aura - damage one
        // char *name = generate_name();
        // todo: perhaps life aura can effect character themself!
        entity health = 0;
        entity energy = 0;
        entity mana = 0;
        for (int j = 0; j < stats->length; j++) {
            entity stat = stats->value[j];
            if (zox_has(stat, StatState)) {
                if (!health) {
                    health = stat;
                } else if (!energy) {
                    energy = stat;
                } else if (!mana) {
                    mana = stat;
                }
            }
        }

        {
            color ecolor = (color) { 155, 155, 155, 255 };
            entity e2 = spawn_realm_skill_melee(world, e, prefab_skill_melee, "punch", "punch", ecolor, 1, 2, 2, energy, 1, 0.35f, 0.15f);
            add_to_SkillLinks(skills, e2);
            meta_skill_punch = e2;
        }

        {
            color ecolor = (color) { 5, 5, 5, 122 };
            float damage = -base_death_aura_damage;
            float range = base_death_aura_range;
            entity e2 = spawn_realm_skill_aura(world, e, prefab_skill_aura, "death aura", "aura_death", ecolor, damage, range, 0.5f, 0.5f);
            add_to_SkillLinks(skills, e2);
            meta_skill_aura_death = e2;
        }

        // testing
        /*{
            entity mana_punch = spawn_skill_melee_t(
                world,
                "mana punch",
                2,
                6,
                4,
                mana,
                3,
                "punch",
                1,
                2);
            add_to_SkillLinks(skills, mana_punch);
        }*/

        /*meta_skill_aura_life = spawn_skill_aura_t(world,
            "life aura",
            base_death_aura_damage,
            base_death_aura_range,
            (color) { 255, 255, 255, 88 },
            "aura_life",
            0.5f,
            0.5f
        );
        // add_to_SkillLinks(skills, meta_skill_aura_life);
        meta_skill_aura_fire = spawn_skill_aura_t(world,
            "fire aura",
            -base_death_aura_damage * 0.5f,
            base_death_aura_range * 4,
            (color) { 255, 22, 22, 144 },
            "aura_fire",
            0.5f,
            0.5f
        );
        // add_to_SkillLinks(skills, meta_skill_aura_fire);*/

        zox_logv("At [%f] Realm [skills] [%i] spawned.", zox_current_time, skills->length);
    }
} zox_sys_end(SkillsRealmSpawnSystem);
