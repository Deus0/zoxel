void quest_on_slay(
    ecs* world,
    entity e,
    entity killer,
    entity dyer
) {
    if (!zox_has(e, SlayQuest)) {
        zox_log("Quest not a SlayQuest [%s]", zox_get_name(e));
        return;
    }
    zox_geter_value(e, CharacterLink, entity, target_character);
    zox_geter_value(dyer, CharacterMetaLink, entity, dyer_meta);
    if (target_character == dyer_meta) {
        zox_muter(e, QuestValue, value);
        zox_geter(e, QuestTarget, target);
        if (value->value < target->value) {
            value->value++;
            zox_log("Incrementing Slay Count [%s]", zox_get_name(target_character));
        } else {
            zox_log("Slay Count Maxxed [%s]", zox_get_name(target_character));
        }
    } else {
        zox_log("Not Slay Target [%s] != [%s]", zox_get_name(target_character), zox_get_name(dyer_meta));
    }
}

zox_sys2(SlaySystem) {
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
        if (!zox_valid(enemy->value) || zox_gett_value(enemy->value, Dead)) {
            continue;
        }

        // add experience to soul stat
        zox_geter(enemy->value, QuestLinks, enemy_quests);

        for (int j = 0; j < enemy_quests->length; j++) {
            entity quest = enemy_quests->value[j];
            quest_on_slay(world, quest, enemy->value, e);
        }
    }
} zox_sys_end(SlaySystem);
