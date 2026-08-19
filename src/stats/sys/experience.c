zox_sys2(ExperienceSystem) {
    const float popup_spawn_y = 0.34f;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Dead);
    zox_sys_in(LastDamager);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Dead, dead);
        zox_sys_i(LastDamager, enemy);
        // proces if i die
        if (dead->value != zox_dirty_active) {
            continue;
        }
        // if enemy invalid or dead, continue
        if (!zox_valid(enemy->value) || zox_getv(enemy->value, Dead)) {
            continue;
        }
        entity my_soul = zox_get_child_by_id(world, e, zox_id(StatSoul));
        if (!zox_valid(my_soul)) {
            continue;
        }
        entity enemy_soul = zox_get_child_by_id(world, enemy->value, zox_id(StatSoul));
        if (!zox_valid(enemy_soul)) {
            continue;
        }
        /*entity stats[stats_children_capacity];
        uint stats_length = zox_get_children(world, e, stats, stats_children_capacity);
        if (!stats_length) {
            zox_logw("[%s] has no stats", zox_get_name(e);
            continue;
        }*/

        // NOTE: Adds experience to soul stat
        /*entity enemy_stats[layouts2_children_capacity];
        uint enemy_stats_length = zox_get_children(world, enemy->value, enemy_stats, stats_children_capacity);
        if (!enemy_stats_length) {
            return;
        }
        // zox_geter(enemy->value, StatLinks, enemy_stats);
        find_array_element_with_tag(stats, StatSoul, my_soul)
        find_array_element_with_tag(enemy_stats, StatSoul, enemy_soul)*/
        // if either player has no soul, xp cannot be transferred
        zox_geter(my_soul, StatValue, my_level);
        zox_muter(enemy_soul, ExperienceValue, experience);
        // base xp
        float experience_gain = experience_gain_base + my_level->value * randf_range(experience_per_level_min, experience_per_level_max);
        // add to victor
        experience->value += experience_gain;
        // zox_log(" + [%s] has gained [%f] xp", zox_get_name(enemy->value), experience_gain)
        float3 bounds3D = zox_getv(e, Bounds3D);
        float3 position = zox_getv(e, Position3D);
        float3 popup_position = (float3) { position.x, position.y + bounds3D.y + popup_spawn_y, position.z };
        char popup_text[64];
        sprintf(popup_text, "+%i", (int) ceil(experience_gain));
        color popup_color = (color) { 255, 255, 0, 255 };
        spawn_popup3_easy(world, popup_text, popup_color, popup_position, zox_popup_scale * 2, 7 + rand() % 3);

    }
} zox_sys_end(ExperienceSystem);
