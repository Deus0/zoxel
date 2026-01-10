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

        zox_geter(realm->value, DialoguetreeLinks, trees);
        tree->value = trees->value[rand() % trees->length];
    }
} zox_sys_end(CharacterDialogueSystem);