// TODO: Spawn UI on keypress, with Node Process - cycle through tree with key presses too

entity test_dialogue_run;
entity test_dialogue_ui;

void key_down_toggle_dialogue(ecs *world, int32_t keycode) {
    if (keycode == SDLK_v) {
        if (zox_valid(test_dialogue_run)) {

            // if not end, go to next:
            zox_geter_value(test_dialogue_run, NodeLink, entity, next);
            if (zox_valid(next)) {
                zox_log("Moving to Next Dialogue Leaf [%s]", zox_get_name(next));
                zox_set(test_dialogue_run, NodeEnd, { zox_dirty_trigger });
            } else {
                zox_log("Dialogue Finished, Ending.");
                zox_delete(test_dialogue_run);
                zox_delete(test_dialogue_ui);
                // delete_nodes(world, test_dialogue_tree);
            }
        } else {
            const entity player = zox_players[0];
            if (!player) {
                zox_log_error("Player is null");
                return;
            }

            zox_log("Testing Realm Dialogue Run [0]");

            // Here is our test run!
            zox_geter_value(player, GameLink, entity, game);
            zox_geter_value(game, RealmLink, entity, realm);
            if (!zox_valid(realm)) {
                zox_logw("No realm yet.");
                return;
            }

            zox_geter(realm, DialoguetreeLinks, dialogues);
            if (!dialogues->length) {
                zox_logw("No Dialoguetrees yet.");
                return;
            }

            zox_geter_value(player, CharacterLink, entity, character);

            entity tree = dialogues->value[0];

            test_dialogue_run = spawn_dialogue_run(
                world,
                prefab_dialogue_run,
                tree,
                0,
                0
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

            zox_set(character, DialogueRunLink, { test_dialogue_run });
        }
    }
}