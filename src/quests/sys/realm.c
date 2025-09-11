entity meta_quest_slay_slems;

void spawn_realm_quests(
    ecs *world,
    const entity realm
) {
    if (!zox_has(realm, QuestLinks)) {
        zox_log("! realm does not have QuestLinks [%lu]\n", realm)
        return;
    }

    zox_geter(realm, QuestLinks, old)
    if (old) {
        for (int i = 0; i < old->length; i++) {
            if (old->value[i]) {
                zox_delete(old->value[i])
            }
        }
    }

    QuestLinks quests = (QuestLinks) { 0 };

    // slay them dirty slems
    {
        meta_quest_slay_slems = spawn_meta_quest(world, prefab_quest, "find bob");
        // meta_quest_slay_slems = spawn_meta_quest(world, prefab_quest, "slay slems");
        // set quest objectives
        // zox_set(meta_quest_slay_slems, TextureLink, { files_textures[12] });
        zox_set(meta_quest_slay_slems, TextureLink, { string_hashmap_get(files_hashmap_textures, new_string_data("Discord")) });
        add_to_QuestLinks(&quests, meta_quest_slay_slems);
    }

    zox_set_ptr(realm, QuestLinks, quests);

    zox_logv("At [%f] Realm [quests] [%i] spawned.", zox_current_time, quests.length);
}