// When health goes to 0, kill UserLink->value
// Set Dead to 1
zox_sys2(CharacterActionsSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        if (state->value != zox_dirty_active) {
            continue;
        }
        // skill!
        entity actionbar = zox_get_child_by_id(world, e, zox_id(Actionbar));
        // TODO: Find a punch skill at level 1
        if (meta_skill_punch) {
            entity e2 = spawn_user_skill(world, e, meta_skill_punch);
            if (zox_valid(actionbar)) {
                entity slot = zox_get_empty_slot(world, actionbar);
                if (zox_valid(slot)) {
                    zox_muter(slot, DataLink, slot_data);
                    slot_data->value = e2;
                }
            }
        }
        // If Player, Fill with Blank!
        /*if (zox_has(e, PlayerLink)) {
            for (int j = actions->length; j < 8; j++) {
                add_to_ActionLinks(actions, 0);
            }
        }*/

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
} zox_sys_end(CharacterActionsSpawnSystem);
