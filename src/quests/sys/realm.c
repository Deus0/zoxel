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

    zox_geter(realm, CharacterLinks, characters);

    QuestLinks quests = (QuestLinks) { 0 };

    // slay them dirty slems
    {
        entity quest = spawn_meta_quest(world, prefab_quest, "find bob");
        // TODO: Object: Find NPC named Bob

        // zox_set(quest, TextureLink, { files_textures[12] });
        entity texture = string_hashmap_get(files_hashmap_textures, new_string_data("discord"));
        zox_set(quest, TextureLink, { texture });
        add_to_QuestLinks(&quests, quest);
    }

    {
        entity quest = spawn_meta_quest(world, prefab_quest, "slay slems");

        // TODO: Objective: Slay 10 Slimes
        zox_add_tag(quest, SlayQuest);
        zox_set(quest, QuestValue, { 0 });
        zox_set(quest, QuestTarget, { 10 });
        entity slime = characters->value[0];
        zox_set(quest, CharacterLink, { slime });

        entity texture = string_hashmap_get(files_hashmap_textures, new_string_data("taskbar_lore"));
        zox_set(quest, TextureLink, { texture });
        add_to_QuestLinks(&quests, quest);
    }

    zox_set_ptr(realm, QuestLinks, quests);

    zox_logv("At [%f] Realm [quests] [%i] spawned.", zox_current_time, quests.length);
}