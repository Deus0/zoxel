void link_dialogue_run_to_ui(
    ecs* world,
    entity run,
    entity ui)
{
    zox_link(world, ui, DialogueProcess, run);
    zox_link(world, run, DialogueUI, ui);
}

void on_click_dialogue_button(
    ecs *world,
    ClickEventData data)
{
    // get window
    entity window = zox_get_parent_by_id(
        world,
        data.clicked,
        zox_id(Window));
    if (!zox_valid(window)) {
        zox_loge("Invalid Dialogue Window [%s]",
            zox_getn(window));
        return;
    }
    entity process = zox_get_link(world, window, DialogueProcess);
    if (!zox_valid(process)) {
        zox_log("Window [%s] has Invalid dialogue_run [%s]",
            zox_getn(window),
            zox_getn(process));
        return;
    }
    zox_setv(process, NodeEnd, zox_dirty_trigger);
}

void on_closed_dialogue_ui(ecs* world, ClickEventData data) {
    // get window
    entity window = zox_get_parent_by_id(
        world,
        data.clicked,
        zox_id(Window));
    if (!zox_valid(window)) {
        zox_loge("Invalid Dialogue Window [%s]",
            zox_getn(window));
        return;
    }
    entity runner = zox_get_link(world, window, DialogueProcess);
    // entity runner = zox_getv(window, DialogueProcessLink);
    if (!zox_valid(runner)) {
        zox_loge("[on_closed_dialogue_ui] runner Invalid");
        return;
    }
    // exit tree when closed window
    zox_setv(runner, NodetreeEnd, zox_dirty_trigger);
}
