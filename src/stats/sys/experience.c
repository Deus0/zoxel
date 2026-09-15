
zox_sys2(ExperienceSystem) {
    const float popup_spawn_y = 0.34f;
    zox_sys_world();
    // zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity attacker = zox_get_link(world, e, LastDamager);
        // proces if i die
        /*if (dead->value != zox_dirty_active) {
            continue;
        }*/
        // if enemy invalid or dead, continue
        if (!zox_valid(attacker) ||
            !zox_has(attacker, Dead))
        {
            continue;
        }
        entity my_soul = zox_get_child_by_id(
            world,
            e,
            zox_id(StatSoul));
        if (!zox_valid(my_soul)) {
            continue;
        }
        entity enemy_soul = zox_get_child_by_id(
            world,
            attacker,
            zox_id(StatSoul));
        if (!zox_valid(enemy_soul)) {
            continue;
        }
        // if either player has no soul, xp cannot be transferred
        zox_geter(my_soul, StatValue, my_level);
        zox_muter(enemy_soul, ExperienceValue, experience);
        // base xp
        float experience_gain = experience_gain_base + my_level->value * randf_range(experience_per_level_min, experience_per_level_max);
        // add to victor
        experience->value += experience_gain;
        float3 bounds3D = zox_getv(e, Bounds3D);
        float3 position = zox_getv(e, Position3D);
        float3 popup_position = (float3) {
            position.x,
            position.y + bounds3D.y + popup_spawn_y,
            position.z
        };
        char popup_text[64];
        sprintf(popup_text, "+%i", (int) ceil(experience_gain));
        color popup_color = (color) { 255, 255, 0, 255 };
        spawn_popup3_easy(
            world,
            popup_text,
            popup_color,
            popup_position,
            zox_popup_scale * 2,
            7 + rand() % 3);
        // zox_log(" + [%s] has gained [%f] xp", zox_get_name(attacker), experience_gain)
    }
} zox_sys_end(ExperienceSystem);
