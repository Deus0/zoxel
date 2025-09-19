// Early Exit from Dialogue
void DialogueEndSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TriggerActionE);
    zox_sys_in(PlayerLink);
    zox_sys_out(DialogueRunLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TriggerActionE, state);
        zox_sys_i(PlayerLink, player);
        zox_sys_o(DialogueRunLink, run);

        if (state->value != zox_dirty_active) {
            continue;
        }

        zox_geter_value(player->value, PlayerState, byte, player_state);
        if (player_state == zox_player_state_dialogue_active) {

            zox_geter_value(run->value, DialogueUILink, entity, ui);
            zox_geter(run->value, SpeakerLinks, speakers);
            zox_delete(ui);
            zox_delete(run->value);
            zox_set(player->value, PlayerState, { zox_player_state_dialogue_end });

            run->value = 0;

            zox_log("End talking to: %s", "idk"); // zox_get_name(raycast->chunk));

            unfollow(world, speakers->value[1]);
        }
    }
} zoxd_system2(DialogueEndSystem);