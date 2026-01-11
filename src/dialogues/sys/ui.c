// DialogueUILink
zox_sys2(DialogueSpeechSystem) {
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
        // zox_log("Node [%s]: Target Speech [%s]", zox_get_name(node->value), text->value);

        set_TargetText(world, speech, text->value);
        /*TargetText target = { };
        memcpy(target.value, text->value, 512);
        zox_set_ptr(speech, TargetText, target);*/

        zox_set(speech, AnimateTextBegin, { zox_current_time });
        zox_set(speech, AnimateTextTime, { 3 });
    }
} zox_sys_end(DialogueSpeechSystem);