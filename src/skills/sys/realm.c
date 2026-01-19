zox_sys2(SkillsRealmSpawnSystem) {
    float base_death_aura_damage = 1;
    float base_death_aura_range = 3;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(StatLinks);
    zox_sys_out(SkillLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(StatLinks, stats);
        zox_sys_o(SkillLinks, skills);

        if (state->value != zox_generate_realm_stats) {
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

        meta_skill_punch = spawn_skill_melee_t(
            world,
            "punch",
            1,
            2,
            2,
            energy,
            1,
            "punch",
            0.25f,
            0.25f
        );

        // testing

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
        // zox_set(meta_skill_punch, TextureLink, { string_hashmap_get(files_hashmap_textures, new_string_data("punch")) })

        meta_skill_aura_death = spawn_skill_aura_t(world,
            "death aura",
            -base_death_aura_damage,
            base_death_aura_range,
            (color) { 5, 5, 5, 122 },
            "aura_death",
            0.5f,
            0.5f
        );
        meta_skill_aura_life = spawn_skill_aura_t(world,
            "life aura",
            base_death_aura_damage,
            base_death_aura_range,
            (color) { 255, 255, 255, 88 },
            "aura_life",
            0.5f,
            0.5f
        );
        meta_skill_aura_fire = spawn_skill_aura_t(world,
            "fire aura",
            -base_death_aura_damage * 0.5f,
            base_death_aura_range * 4,
            (color) { 255, 22, 22, 144 },
            "aura_fire",
            0.5f,
            0.5f
        );

        add_to_SkillLinks(skills, meta_skill_punch);
        add_to_SkillLinks(skills, mana_punch);
        add_to_SkillLinks(skills, meta_skill_aura_death);
        add_to_SkillLinks(skills, meta_skill_aura_life);
        add_to_SkillLinks(skills, meta_skill_aura_fire);

        zox_logv("At [%f] Realm [skills] [%i] spawned.", zox_current_time, skills->length);
    }
} zox_sys_end(SkillsRealmSpawnSystem);