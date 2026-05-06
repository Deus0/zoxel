extern entity game_start_player_new(ecs*, entity, float3* spawned);

zox_sys2(Player3RespawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(PlayerState);
    zox_sys_out(PlayerRespawn);
    zox_sys_out(CharacterLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(PlayerState, state);
        zox_sys_o(PlayerRespawn, respawn);
        zox_sys_o(CharacterLink, character);

        // what happened HHere...? NANI?
        if (respawn->value > 0 && zox_valid(character->value)) {
            zox_log("Respawn [cancel] (character alive)");
            respawn->value = 0;
            state->value = zox_player_state_playing;
        }

        // Player State Changes
        else if (respawn->value > 0 && state->value != zox_player_state_respawning) {
            zox_log("Respawn [cancel] (player state)");
            respawn->value = 0;
        }

        // Playing happily!
        else if (state->value == zox_player_state_playing) {
            if (zox_valid(character->value) && (!zox_has(character->value, Dead) || !zox_gett_value(character->value, Dead))) {
                continue;
            }
            // start respawn timer!
            //  + active red overlay
            zox_log("Respawn [begin] (character dead)");
            state->value = zox_player_state_respawning;
            respawn->value = respawn_time;
        }

        // What happens after death??
        else if (state->value == zox_player_state_respawning) {
            respawn->value -= zox_delta_time;
            if (respawn->value <= 0) {
                zox_log("Respawn [activates]");
                state->value = zox_player_state_playing;
                float3 spawned;
                character->value = game_start_player_new(world, e, &spawned);
            }
        }
    }
} zox_sys_end(Player3RespawnSystem);
