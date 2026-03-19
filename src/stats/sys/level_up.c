// purely for detecting level up

// using level up state, we spawn sound and particles


zox_sys2(LevelUpSystem) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(UserLink);
    zox_sys_out(StatValue);
    zox_sys_out(ExperienceValue);
    zox_sys_out(ExperienceMax);
    zox_sys_out(StatDirty);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(UserLink, userLink);
        zox_sys_o(StatValue, statValue);
        zox_sys_o(ExperienceValue, experienceValue);
        zox_sys_o(ExperienceMax, experienceMax);
        zox_sys_o(StatDirty, dirty);

        if (experienceValue->value < experienceMax->value) {
            continue;
        }

        experienceValue->value -= experienceMax->value;
        experienceMax->value = (int) ( experienceMax->value * experience_max_multiplier);
        statValue->value++;
        // zox_log_name(" + [%s] has leveled up", userLink->value)

        // give 1 in all attributes for now
        //  to test them out
        // TODO: Increase StatPoint and SkillPoint instead
        zox_geter(userLink->value, StatLinks, stats);
        for (int j = 0; j < stats->length; j++) {
            const entity stat = stats->value[j];
            if (zox_has(stat, StatAttribute)) {
                zox_muter(stat, StatValue, attribute);
                zox_muter(stat, StatDirty, attribute_dirty);
                attribute->value++;
                attribute_dirty->value = zox_dirty_trigger;
            }
        }

        // level up sound
        spawn_sound_generated(
            world,
            prefab_sound_generated,
            instrument_piano,
            note_frequencies[32 + rand() % 4], 3.4, 1.4f * get_volume_sfx());

        // spawn particle system
        zox_muter(userLink->value, Children, children);
        float3 bounds = zox_get_value(userLink->value, Bounds3D);

        entity e2 = spawn_particle3D_emitter(world, userLink->value, 10, float3_scale(bounds, 3), (color) { 255, 255, 0, 55 });
        zox_set(e2, DestroyInTime, { 3 + statValue->value });
        add_to_Children(children, e2);

        dirty->value = zox_dirty_trigger;

    }
} zox_sys_end(LevelUpSystem);
