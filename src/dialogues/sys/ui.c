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

        if (state->value != zox_dirty_active) {
            continue;
        }

        if (!zox_valid(ui->value) || !zox_has(ui->value, DialogueTextLink)) {
            continue;
        }

        zox_geter_value(ui->value, DialogueTextLink, entity, e2);

        if (!zox_valid(e2)) {
            continue;
        }

        /*entity children[layouts2_children_capacity];
        uint children_length = zox_get_children(world, ui->value, children, layouts2_children_capacity);
        // zox_geter(ui->value, Children, children);
        if (children_length < 2) {
            continue;
        }*/

        // entity speech = children[1];

        zox_geter(node->value, DialogueText, text);
        set_TargetText(world, e2, text->value);
        zox_set(e2, AnimateTextBegin, { zox_current_time });
        zox_set(e2, AnimateTextTime, { 3 });

        // zox_log("Node [%s]: Target Speech [%s]", zox_get_name(node->value), text->value);
    }
} zox_sys_end(DialogueSpeechSystem);
