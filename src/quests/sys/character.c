zox_sys2(CharacterPlayerQuestsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RealmLink);
    // zox_sys_out(QuestLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        // zox_sys_o(QuestLinks, quests);
        if (state->value != zox_dirty_active) {
            continue;
        }
        // get voxels
        zox_geter(realm->value, QuestLinks, realm_quests);
        if (!realm_quests->length) {
            continue;
        }
        entity player_start_quest = realm_quests->value[1];
        if (!zox_valid(player_start_quest)) {
            return;
        }
        //  TODO: Go through quest objectives, if Slay objective, add to characters SlayEvent component, storing entity and function
        // TODO: When character slay, itll check objective inside the hook function, did player kill slime? itll compare the character with objective data
        {
            spawn_user_quest(world, e, player_start_quest);
            // entity quest = spawn_user_quest(world, player_start_quest, e);
            // add_to_QuestLinks(quests, quest);
        }
    }
} zox_sys_end(CharacterPlayerQuestsSystem);
