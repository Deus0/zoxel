// left click - destroy
void DialogueBeginSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TriggerActionB);
    zox_sys_in(RaycastVoxelData);
    zox_sys_in(PlayerLink);
    zox_sys_out(DialogueRunLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TriggerActionB, state);
        zox_sys_i(RaycastVoxelData, raycast);
        zox_sys_i(PlayerLink, player);
        zox_sys_o(DialogueRunLink, run);

        if (state->value != zox_dirty_active) {
            continue;
        }

        if (raycast->result != rayhit_character || !zox_valid(raycast->chunk)) {
            continue;
        }

        zox_geter_value(player->value, PlayerState, byte, player_state);

        // zox_player_state_dialogue_active
        if (player_state == zox_player_state_playing) {
            entity npc = raycast->chunk;

            zox_geter_value(npc, CombatState, byte, combat);
            if (combat != zox_combat_peace) {
                zox_log("Cannot talk in combat with: %s", zox_get_name(npc));
                continue;
            }

            zox_geter_value(npc, DialoguetreeLink, entity, tree);
            if (!zox_valid(tree)) {
                zox_log("NPC has no speech: %s", zox_get_name(npc));
                continue;
            }

            // zox_log("Begin talking to: %s", zox_get_name(npc));

            /*zox_geter_value(player->value, GameLink, entity, game);
            zox_geter_value(game, RealmLink, entity, realm);
            if (!zox_valid(realm)) {
                zox_logw("No realm.");
                continue;
            }
            zox_geter(realm, DialoguetreeLinks, dialogues);
            if (!dialogues->length) {
                zox_logw("No Dialoguetrees.");
                continue;
            }*/

            // TODO: Get Dialogue off NPC
            // TODO: Link UI to player
            // TODO: Set NPC State
            // TODO: Link Run to Player
            // entity dialogue = dialogues->value[0];
            run->value = spawn_dialogue_run(
                world,
                prefab_dialogue_run,
                tree,
                e,
                npc
            );
            entity dialogue_ui = spawn_dialogue_ui(
                world,
                prefab_dialogue_ui,
                player->value
            );
            link_dialogue_run_to_ui(
                world,
                run->value,
                dialogue_ui
            );

            zox_set(player->value, PlayerState, { zox_player_state_dialogue_begin });

            follow_target(world, npc, e);

            // zox_log("Character [%s] spawned dialogue_run [%lu]", zox_get_name(e), run->value);
        }

    }
} zoxd_system2(DialogueBeginSystem);