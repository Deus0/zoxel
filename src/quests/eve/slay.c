void check_quest_slay(
    ecs* world,
    entity quest,
    entity meta,
    byte dbg_log)
{
    entity target = zox_get_link(world, quest, CharacterLink);
    if (!target) {
        zox_loge("Slay Quest [%s] No Target Character.",
            zox_getn(quest));
        return;
    }
    if (target == meta) {
        zox_muter(quest, QuestValue, value);
        zox_geter(quest, QuestTarget, target_value);
        if (value->value < target_value->value) {
            value->value++;
            zox_add(quest, Dirty);
            zox_add(quest, DataDirty);
            // if complete, we should set full here
            if (dbg_log) {
                zox_log("Incrementing Quest [%s]'s Slay Count [%s]",
                    zox_getn(quest),
                    zox_getn(target));
            }
            if (value->value == target_value->value) {
                zox_add(quest, QuestDone);
                if (dbg_log) {
                    zox_log("Quest [%s]'s Slay Count Maxxed [%s]",
                        zox_getn(quest),
                        zox_getn(target));
                }
            }
        }
    } else {
        if (dbg_log) {
            zox_log("   - Not Slay Target [%s] != [%s]",
                zox_getn(target),
                zox_getn(meta));
        }
    }
}

void zox_event_quest_slay(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity attacker = zox_get_link(world, e, AttackerLink);
        if (!attacker ||
            zox_has(attacker, Dead))
        {
            if (dbg_log) {
                zox_log("[zox_event_quest_slay] Attacker Not Found on [%s]",
                    zox_sys_e_name);
            }
            continue;
        }
        entity slayed_character_prefab = zox_getp(world, e);
        // for all attacks quests
        iter it2 = zox_children(world, attacker);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity child = it2.entities[j];
                if (!zox_has(child, Quest) ||
                    !zox_has(child, SlayQuest)
                ) {
                    continue;
                }
                if (dbg_log) {
                    zox_log("[zox_event_quest_slay] Checking [%s]'s Slay Quest [%s]",
                        zox_getn(attacker),
                        zox_getn(child));
                }
                check_quest_slay(
                    world,
                    child,
                    slayed_character_prefab,
                    dbg_log
                );
            }
        }
    }
}
