// DialogueUILink
zox_sys2(DialogueSpeechSystem) {
    byte dbg_log = 1;
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
        entity text_ui = zox_getv(ui->value, DialogueTextLink);
        entity next_button = zox_get_child_by_id_recursive(world, ui->value, zox_id(DialogueButton));
        if (!zox_valid(text_ui) || !zox_valid(next_button)) {
            continue;
        }
        zox_geter(node->value, DialogueText, text);
        set_TargetText(world, text_ui, text->value);
        zox_muter(text_ui, TextData, text_data);
        resize_TextData(text_data, 0);
        zox_set(text_ui, TextDirty, { zox_dirty_trigger });
        zox_set(text_ui, AnimateTextBegin, { zox_current_time });
        // zox_set(next_button, RenderDisabled, { 1 });
        set_children_by_id_byte(world, next_button, zox_id(RenderDisabled), 1);
        if (dbg_log) {
            zox_log("Dialogue Node [%s], Text UI [%s], Next Button [%s], Target Text [%s]", zox_get_name(node->value), zox_get_name(text_ui), zox_get_name(next_button), text->value);
        }
    }
} zox_sys_end(DialogueSpeechSystem);
