// Links a npc to a dialogue!
zox_sys2(CharacterDialogueSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RealmLink);
    zox_sys_out(DialoguetreeLink);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        zox_sys_o(DialoguetreeLink, tree);
        if (state->value != zox_dirty_active) {
            continue;
        }
        uint capacity = 256;
        entity trees[capacity];
        uint length = zox_get_children_by_id(
            world,
            realm->value,
            trees,
            capacity,
            zox_id(Dialogue));
        if (!length) {
            zox_loge("No dialogue found on realm [%s]",
                zox_getn(realm->value));
            continue;
        }
        tree->value = trees[rand() % length];
    }
} zox_sys_end(CharacterDialogueSystem);
