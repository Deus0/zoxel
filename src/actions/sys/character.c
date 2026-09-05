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
    }
} zox_sys_end(CharacterActionsSpawnSystem);
