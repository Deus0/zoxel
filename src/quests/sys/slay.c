void quest_on_slay(
    ecs* world,
    entity e,
    entity killer,
    entity slayed)
{
    if (!zox_has(e, SlayQuest)) {
        zox_logv("Quest not a SlayQuest [%s]", zox_get_name(e));
        return;
    }
    entity target = zox_get_link(world, e, Character);
    entity meta = zox_getp(world, slayed);
    if (target == meta) {
        zox_muter(e, QuestValue, value);
        zox_geter(e, QuestTarget, target_value);
        if (value->value < target_value->value) {
            value->value++;
            zox_muter(e, QuestDirty, qdirty);
            qdirty->value = zox_dirty_trigger;
            zox_logv("Incrementing Slay Count [%s]", zox_get_name(target));
        } else {
            zox_logv("Slay Count Maxxed [%s]", zox_get_name(target));
        }
    } else {
        zox_logv("Not Slay Target [%s] != [%s]", zox_get_name(target), zox_get_name(meta));
    }
}

zox_sys2(SlaySystem) {
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // if enemy invalid or dead, continue
        entity attacker = zox_get_link(world, e, LastDamager);
        if (!zox_valid(attacker) ||
            zox_has(attacker, Dead))
        {
            continue;
        }
        // add experience to soul stat
        // Others
        entity enemy_quests[zox_children_capacity];
        uint enemy_quests_length = zox_get_children_by_id(
            world,
            attacker,
            enemy_quests,
            zox_children_capacity,
            zox_id(Quest));
        for (uint j = 0; j < enemy_quests_length; j++) {
            entity quest = enemy_quests[j];
            quest_on_slay(
                world,
                quest,
                attacker,
                e);
        }
    }
} zox_sys_end(SlaySystem);
