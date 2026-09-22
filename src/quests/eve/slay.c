void quest_slay_check(
    ecs* world,
    entity quest,
    entity slayed_character_prefab)
{
    entity target = zox_get_link(world, quest, CharacterLink);
    if (!target) {
        zox_loge("Slay Quest [%s] No Target Character.",
            zox_getn(quest));
        return;
    }
    if (target == slayed_character_prefab) {
        zox_muter(quest, QuestValue, value);
        zox_geter(quest, QuestTarget, target_value);
        if (value->value < target_value->value) {
            value->value++;
            zox_add(quest, Dirty);
            zox_add(quest, DataDirty);
            // zox_muter(quest, QuestDirty, qdirty);
            // qdirty->value = zox_dirty_trigger;
            // if complete, we should set full here
            zox_logv("Incrementing Slay Count [%s]",
                zox_get_name(target));
            if (value->value == target_value->value) {
                zox_logv("Slay Count Maxxed [%s]",
                    zox_get_name(target));
                zox_add(quest, QuestDone);
            }
        }
    } else {
        zox_logv("Not Slay Target [%s] != [%s]",
            zox_getn(target),
            zox_getn(meta));
    }
}

void zox_event_quest_slay(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity attacker = zox_get_link(world, e, LastDamager);
        if (!attacker ||
            !zox_has(attacker, CharacterLink))
        {
            continue;
        }
        entity slayed_character_prefab = zox_getp(world, e);
        // for all attacks quests
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity child = it2.entities[j];
                if (!zox_has(child, Quest) ||
                    !zox_has(child, SlayQuest)
                ) {
                    continue;
                }
                quest_slay_check(
                    world,
                    child,
                    slayed_character_prefab
                );
            }
        }
    }
}