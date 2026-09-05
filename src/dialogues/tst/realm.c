// TODO: Spawn UI on keypress, with Node Process - cycle through tree with key presses too
entity test_dialogue_run;
entity test_dialogue_ui;

void zox_tst_spawn_dialogue(ecs *world, ClickEventData data) {
    if (zox_valid(test_dialogue_run)) {
        // if not end, go to next:
        entity next = zox_getv(test_dialogue_run, NodeLink);
        if (zox_valid(next)) {
            zox_log("Moving to Next Dialogue Leaf [%s]",
                zox_getn(next));
            zox_setv(test_dialogue_run, NodeEnd, zox_dirty_trigger);
        } else {
            zox_log("Dialogue Finished, Ending.");
            zox_delete(test_dialogue_run);
            zox_delete(test_dialogue_ui);
        }
        return;
    }
    entity player = dbg_player;
    if (!player) {
        zox_loge("Player is null");
        return;
    }
    zox_log("Testing Realm Dialogue Run [0]");
    // Here is our test run!
    entity game = zox_getv(player, GameLink);
    entity realm = zox_getv(game, RealmLink);
    if (!zox_valid(realm)) {
        zox_logw("No realm yet.");
        return;
    }
    entity tree = zox_get_child_by_id(
        world,
        realm,
        zox_id(Dialogue));
    if (!zox_valid(realm)) {
        zox_logw("Invalid [tree] on Realm");
        return;
    }
    entity canvas = zox_get_link(world, player, Canvas);
    entity character = zox_get_link(world, player, Character);
    if (!zox_valid(character)) {
        zox_logw("Player has no character.");
        return;
    }
    // entity tree = dialogues->value[0];
    test_dialogue_run = spawn_process_dialogue(
        world,
        prefab_process_dialogue,
        tree,
        0,
        0);
    test_dialogue_ui = spawn_dialogue_ui(
        world,
        canvas,
        character,
        0);
    link_dialogue_run_to_ui(
        world,
        test_dialogue_run,
        test_dialogue_ui);
    zox_link(
        world,
        test_dialogue_ui,
        DialogueProcess,
        test_dialogue_run);
    zox_link(
        world,
        character,
        DialogueProcess,
        test_dialogue_run);
    // zox_setv(test_dialogue_ui, DialogueProcessLink, test_dialogue_run);
    // zox_setv(character, DialogueProcessLink, test_dialogue_run);
}
