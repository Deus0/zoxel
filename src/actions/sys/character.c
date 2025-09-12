// When health goes to 0, kill UserLink->value
// Set Dead to 1
void CharacterActionsSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    //zox_sys_in(RealmLink);
    zox_sys_out(ActionLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        //zox_sys_i(RealmLink, realm);
        zox_sys_o(ActionLinks, actions);

        if (state->value != zox_dirty_active) {
            continue;
        }

        // skill!
        if (meta_skill_punch) {
            const entity action = spawn_user_skill(
                world,
                meta_skill_punch,
                e
            );
            add_to_ActionLinks(actions, action);
        }
        // If Player, Fill with Blank!
        if (zox_has(e, PlayerLink)) {
            for (int j = actions->length; j < 8; j++) {
                add_to_ActionLinks(actions, 0);
            }
        }

        /*if (test_actions_skills) {
            if (meta_skill_aura_death) {
                actions->value[place_index++] = spawn_user_skill(world, meta_skill_aura_death, data->e);
            }
            if (meta_skill_aura_life) {
                actions->value[place_index++] = spawn_user_skill(world, meta_skill_aura_life, data->e);
            }
            if (meta_skill_aura_fire) {
                actions->value[place_index++] = spawn_user_skill(world, meta_skill_aura_fire, data->e);
            }
        }
        if (test_actions_blocks) {
            if (meta_item_block_dungeon_core) {
                actions->value[place_index++] = spawn_user_item(world, meta_item_block_dungeon_core, data->e);
            }
            if (meta_item_block_dark) {
                actions->value[place_index++] = spawn_user_item(world, meta_item_block_dark, data->e);
                zox_set(actions->value[place_index - 1], Quantity, { 12 })
            }
        }*/
    }
} zoxd_system2(CharacterActionsSystem);