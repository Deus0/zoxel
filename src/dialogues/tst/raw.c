// TODO: Spawn UI on keypress, with Node Process - cycle through tree with key presses too
entity test_dialogue_run;
entity test_dialogue_ui;
entity test_dialogue_tree;

void zox_tst_spawn_dialogue(ecs *world, ClickEventData data) {
    if (zox_valid(test_dialogue_run)) {
        // if not end, go to next:
        entity next = zox_get_link(world, test_dialogue_run, NodeLink);
        if (zox_valid(next)) {
            zox_log("Moving to Next Dialogue Leaf [%s]",
                    zox_getn(next));
            zox_setv(test_dialogue_run, NodeEnd, zox_dirty_trigger);
        } else {
            zox_log("Dialogue Finished, Ending.");
            zox_delete(test_dialogue_run);
            zox_delete(test_dialogue_ui);
            zox_delete(test_dialogue_tree);
        }
        return;
    }
    entity player = dbg_player;
    if (!player) {
        zox_loge("Player is null");
        return;
    }
    zox_log("Testing Realm Dialogue Run [0]");
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity tree;
    {
        byte texts_count = 3;
        char* texts[] = {
            "Welcome Traveler",
            "Cajeet Greets you.",
            "May the Sun God bless thee.",
        };
        tree = spawn_dialogue_tree_texts(
            world,
            prefab_dialogue_node,
            0,
            texts,
            texts_count).x;
    }
    test_dialogue_run = spawn_process_dialogue(
        world,
        prefab_process_dialogue,
        tree,
        0,
        0);
    test_dialogue_ui = spawn_dialogue_ui(
        world,
        canvas,
        0,
        0);
    link_dialogue_run_to_ui(
        world,
        test_dialogue_run,
        test_dialogue_ui);
    // zox_setv(test_dialogue_ui, DialogueProcessLink, test_dialogue_run);
    zox_link(world, test_dialogue_ui, DialogueProcess, test_dialogue_run);
    //  zox_setv(character, DialogueProcessLink, test_dialogue_run);
    test_dialogue_tree = tree;
}
