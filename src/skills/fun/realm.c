void spawn_realm_skills(
    ecs *world,
    const entity realm
) {
    const float base_death_aura_damage = 1;
    const float base_death_aura_range = 3;

    if (!zox_has(realm, SkillLinks)) {
        zox_log("! realm does not have SkillLinks [%lu]\n", realm)
        return;
    }
    // clear previous
    zox_geter(realm, SkillLinks, old)
    // if (old->value) return; // TODO: Temp; Remove when crashes gone

    if (old) {
        for (int i = 0; i < old->length; i++) {
            if (old->value[i]) {
                zox_delete(old->value[i]);
            }
        }
        // dispose_SkillLinks_const(old);
    }

    zox_geter(realm, StatLinks, stats)
    SkillLinks skills = (SkillLinks) { 0, NULL };
    // aura - damage one
    // char *name = generate_name();
    // todo: perhaps life aura can effect character themself!

    entity health = 0;
    entity energy = 0;
    entity mana = 0;
    for (int i = 0; i < stats->length; i++) {
        const entity stat = stats->value[i];
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

    add_to_SkillLinks(&skills, meta_skill_punch);
    add_to_SkillLinks(&skills, mana_punch);
    add_to_SkillLinks(&skills, meta_skill_aura_death);
    add_to_SkillLinks(&skills, meta_skill_aura_life);
    add_to_SkillLinks(&skills, meta_skill_aura_fire);

    zox_set_ptr(realm, SkillLinks, skills);

    zox_logv("At [%f] Realm [skills] [%i] spawned.", zox_current_time, skills.length);
}