// A NodeRunner will be linked to the UIs
zox_sys2(DialogueSpeechSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity ui = zox_get_link(world, e, DialogueUI);
        if (!zox_valid(ui))
        {
            zox_loge("Dialogue Runner [%s] Invalid UI [%s]",
                zox_sys_e_name,
                zox_getn(ui));
            continue;
        }
        entity text_ui = zox_get_link(world, ui, DialogueLabel);
        entity next_button = zox_get_child_by_id_recursive(
            world,
            ui,
            zox_id(DialogueButton));
        if (!zox_valid(text_ui) || !zox_valid(next_button)) {
            zox_loge("Dialogue Missing Text or Buttons");
            continue;
        }
        entity node = zox_get_link(world, e, CurrentNodeLink);
        if (!node) {
            zox_loge("Current node invalid in dialogue");
            continue;
        }
        const char* text =
            zox_has(node, DialogueText) ?
                zox_getv(node, DialogueText) :
                ".....";
        // zox_geter(node->value, DialogueText, text);
        set_TargetText(world, text_ui, text);
        zox_muter(text_ui, TextData, text_data);
        resize_TextData(text_data, 0);
        zox_add(text_ui, Dirty);
        zox_setv(text_ui, AnimateTextBegin, zox_current_time);
        set_children_by_id_byte(
            world,
            next_button,
            zox_id(RenderDisabled),
            1);
        if (dbg_log) {
            zox_log("Dialogue Node [%s], Text UI [%s], Next Button [%s], Target Text [%s]",
                zox_getn(node),
                zox_getn(text_ui),
                zox_getn(next_button),
                text);
        }
    }
} zox_sys_end(DialogueSpeechSystem);
