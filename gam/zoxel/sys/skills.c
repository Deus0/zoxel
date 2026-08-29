zox_sys2(SkillsRealmSpawnSystem) {
    float base_death_aura_damage = 1;
    float base_death_aura_range = 3;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        if (state->value != zox_generate_realm_skills) {
            continue;
        }
        // aura - damage one
        // char *name = generate_name();
        // todo: perhaps life aura can effect character themself!
        entity health = 0;
        entity energy = 0;
        entity mana = 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity stat = it2.entities[j];
                if (!zox_has(stat, Stat)) {
                    continue;
                }
                if (zox_has(stat, StatState)) {
                    if (!health && zox_has(stat, StatHealth)) {
                        health = stat;
                    } else if (!energy) {
                        energy = stat;
                    } else if (!mana) {
                        mana = stat;
                    }
                }
            }
        }
        // Punch
        {
            color ecolor = (color) { 155, 155, 155, 255 };
            float range = 1.5f;
            float2 damage = (float2) { 1, 2 };
            entity e2 = spawn_realm_skill_melee(
                world,
                e,
                prefab_skill_melee,
                "punch",
                "punch",
                ecolor,
                damage.x,
                damage.y,
                range,
                energy,
                1,
                0.25f,
                0.25f);
            // add_to_SkillLinks(skills, e2);
            meta_skill_punch = e2;
        }
        // Shoot
        {
            color ecolor = (color) { 125, 175, 175, 255 };
            float range = 1.5f;
            float2 damage = (float2) { 1, 2 };
            entity e2 = spawn_realm_skill_shoot(
                world,
                e,
                prefab_skill_shoot,
                "shoot",
                "shoot",
                ecolor,
                damage.x,
                damage.y,
                range,
                energy,
                1,
                0.5f,
                0.5f);
            // add_to_SkillLinks(skills, e2);
        }
        // Aura Skill
        {
            color ecolor = (color) { 5, 5, 5, 122 };
            float damage = -base_death_aura_damage;
            float range = base_death_aura_range;
            entity e2 = spawn_realm_skill_aura(
                world,
                e,
                prefab_skill_aura,
                "death aura",
                "aura_death",
                ecolor,
                damage,
                range,
                0.5f,
                0.5f);
            // add_to_SkillLinks(skills, e2);
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
        zox_logv("Realm [skills] [X] spawned.");
    }
} zox_sys_end(SkillsRealmSpawnSystem);
