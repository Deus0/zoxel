void link_dialogue_run_to_ui(ecs* world, entity run, entity ui) {
    zox_set(ui, DialogueProcessLink, { run });
    zox_set(run, DialogueUILink, { ui });
}

void on_click_dialogue_button(ecs *world, const ClickEventData data) {
    // zox_log("on_click_dialogue_button");
    entity player = data.clicker;
    if (!zox_valid(player) || !zox_has(player, CharacterLink)) {
        zox_log("Invalid player or No CharacterLink");
        return;
    }
    zox_geter_value(player, CharacterLink, entity, character);
    if (!zox_valid(character) || !zox_has(character, DialogueProcessLink)) {
        zox_log("Character invalid or No DialogueProcessLink");
        return;
    }
    zox_geter_value(character, DialogueProcessLink, entity, e);
    if (!zox_valid(e)) {
        zox_log("Character [%s] has Invalid dialogue_run [%lu]", zox_get_name(character), e);
        return;
    }
    zox_set(e, NodeEnd, { zox_dirty_trigger });
}

void on_closed_dialogue_ui(ecs* world, ClickEventData data) {
    entity header = zox_get_parent(world, data.clicked);
    /*if (!zox_has(data.clicked, ParentLink)) {
        zox_log_error("[on_closed_dialogue_ui] close button parent link missing.");
        return;
    }
    zox_geter_value(data.clicked, ParentLink, entity, header);*/
    if (!zox_valid(header)) {
        zox_log_error("[on_closed_dialogue_ui] Header Invalid");
        return;
    }
    entity window = zox_get_parent(world, header);
    // zox_geter_value(header, ParentLink, entity, window);
    if (!zox_valid(window) || !zox_has(window, DialogueProcessLink)) {
        zox_log_error("[on_closed_dialogue_ui] window ParentLink Invalid");
        return;
    }
    zox_geter_value(window, DialogueProcessLink, entity, runner);
    if (!zox_valid(runner)) {
        zox_log_error("[on_closed_dialogue_ui] runner Invalid");
        return;
    }
    // exit tree when closed window
    zox_set(runner, NodetreeEnd, { zox_dirty_trigger });
}
