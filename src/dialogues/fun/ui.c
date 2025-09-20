void link_dialogue_run_to_ui(ecs* world, entity run, entity ui) {
    zox_set(ui, DialogueRunLink, { run });
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
    if (!zox_valid(character) || !zox_has(character, DialogueRunLink)) {
        zox_log("Character invalid or No DialogueRunLink");
        return;
    }

    zox_geter_value(character, DialogueRunLink, entity, dialogue_run);
    if (!zox_valid(dialogue_run)) {
        zox_log("Character [%s] has Invalid dialogue_run [%lu]", zox_get_name(character), dialogue_run);
        return;
    }

    zox_set(dialogue_run, NodeEnd, { zox_dirty_trigger });
}

void on_closed_dialogue_ui(ecs* world, const ClickEventData data) {

    if (!zox_has(data.clicked, ParentLink)) {
        zox_log_error("[on_closed_dialogue_ui] close button parent link missing.");
        return;
    }
    zox_geter_value(data.clicked, ParentLink, entity, header);

    if (!zox_valid(header) || !zox_has(header, ParentLink)) {
        zox_log_error("[on_closed_dialogue_ui] Header Invalid");
        return;
    }
    zox_geter_value(header, ParentLink, entity, window);
    if (!zox_valid(window) || !zox_has(window, DialogueRunLink)) {
        zox_log_error("[on_closed_dialogue_ui] window ParentLink Invalid");
        return;
    }

    zox_geter_value(window, DialogueRunLink, entity, runner);

    if (!zox_valid(runner)) {
        zox_log_error("[on_closed_dialogue_ui] runner Invalid");
        return;
    }

    // exit tree when closed window
    zox_set(runner, NodetreeEnd, { zox_dirty_trigger });
}