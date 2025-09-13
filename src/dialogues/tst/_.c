// TODO: Spawn UI on keypress, with Node Process - cycle through tree with key presses too

entity test_dialogue_tree;
entity test_dialogue_leaf;
entity test_dialogue_run;
entity test_dialogue_ui;

void key_down_toggle_dialogue(ecs *world, int32_t keycode) {
    if (keycode == SDLK_v) {
        if (zox_valid(test_dialogue_tree)) {

            // if not end, go to next:
            zox_geter_value(test_dialogue_run, NodeLink, entity, next);
            if (next) {
                zox_log("Moving to Next Dialogue Leaf");
                zox_set(test_dialogue_run, NodeEnd, { zox_dirty_trigger });
            } else {
                zox_log("Dialogue Finished, Ending.");
                zox_delete(test_dialogue_tree);
                zox_delete(test_dialogue_leaf);
                zox_delete(test_dialogue_run);
                zox_delete(test_dialogue_ui);
            }
        } else {
            const entity player = zox_players[0];
            if (!player) {
                zox_log_error("Player is null");
                return;
            }

            zox_log("Testing Realm Dialogue [0]");

            // TODO: test spawn a tree here and display it on screen ui
            test_dialogue_tree = spawn_dialogue_tree(
                world,
                prefab_dialogue_tree,
                "Hello World."
            );
            test_dialogue_leaf = spawn_dialogue_leaf(
                world,
                prefab_dialogue_leaf,
                "Well, well, well..."
            );

            new_link_single_node(
                world,
                test_dialogue_tree,
                test_dialogue_leaf
            );


            test_dialogue_run = spawn_dialogue_run(
                world,
                prefab_dialogue_run,
                test_dialogue_tree
            );
            test_dialogue_ui = spawn_dialogue_ui(
                world,
                prefab_dialogue_ui,
                player
            );

            link_dialogue_run_to_ui(
                world,
                test_dialogue_run,
                test_dialogue_ui
            );
            // Now Run it
            // Go Through Nodes - key press
        }
    }
}