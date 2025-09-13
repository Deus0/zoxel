// DialogueUILink
// TODO: When Node Begins - Set Dialogue Text

void DialogueSpeechSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeBegin);
    zox_sys_in(NodeLink);
    zox_sys_in(DialogueUILink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(NodeBegin, state);
        zox_sys_i(NodeLink, node);
        zox_sys_i(DialogueUILink, ui);

        if (state->value != zox_dirty_active || !zox_valid(ui->value)) {
            continue;
        }

        zox_geter(ui->value, Children, children);
        if (children->length < 2) {
            continue;
        }

        entity speech = children->value[1];
        zox_geter(node->value, DialogueText, text);
        zox_log("Node [%s]: Speech [%s]",
                zox_get_name(node->value), text->value);

        set_entity_text(world, speech, text->value);
        // zox_muter(speech, TextData, data);
    }
} zoxd_system2(DialogueSpeechSystem);