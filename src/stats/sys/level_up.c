// purely for detecting level up
// using level up state, we spawn sound and particles
zox_sys2(LevelUpSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(StatValue);
    zox_sys_out(ExperienceValue);
    zox_sys_out(ExperienceMax);
    zox_sys_out(StatDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(StatValue, level);
        zox_sys_o(ExperienceValue, experience);
        zox_sys_o(ExperienceMax, experience_max);
        zox_sys_o(StatDirty, dirty);
        if (experience->value < experience_max->value) {
            continue;
        }
        entity parent = zox_get_parent(world, e);
        if (!zox_valid(parent)) {
            continue;
        }
        experience->value -= experience_max->value;
        experience_max->value = (int) ( experience_max->value * experience_max_multiplier);
        level->value++;
        // zox_log_name(" + [%s] has leveled up", userLink->value)
        // give 1 in all attributes for now
        //  to test them out
        // TODO: Increase StatPoint and SkillPoint instead
        entity stats[stats_children_capacity];
        uint stats_length = zox_get_children_by_id(world, parent, stats, stats_children_capacity, zox_id(Stat));
        if (!stats_length) {
            zox_logw("[%s] has no stats", zox_get_name(parent));
            continue;
        }
        for (uint j = 0; j < stats_length; j++) {
            entity stat = stats[j];
            if (!zox_has(stat, StatAttribute)) {
                continue;
            }
            zox_muter(stat, StatValue, attribute);
            zox_muter(stat, StatDirty, attribute_dirty);
            attribute->value++;
            attribute_dirty->value = zox_dirty_trigger;
        }
        // level up sound
        spawn_sound_generated(world, prefab_sound_generated, instrument_piano, note_frequencies[32 + rand() % 4], 3.4, 1.4f * get_volume_sfx());
        // spawn particle system
        float3 bounds = zox_get_value(parent, Bounds3D);
        // Spawns level up particles
        entity e2 = spawn_particle3D_emitter(world, parent, 10, float3_scale(bounds, 3), (color) { 255, 255, 0, 55 });
        zox_set(e2, DestroyInTime, { 3 + level->value });
        // dirty
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(LevelUpSystem);
