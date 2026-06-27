extern entity game_start_player_new(ecs*, entity, entity, entity, entity, float3*, byte);

// NOTE: Detects character death and respawns when needed
// TODO: Move spawning player out and use states instead
zox_sys2(Player3RespawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CameraLink);
    zox_sys_out(PlayerStateDirty);
    zox_sys_out(PlayerState);
    zox_sys_out(CharacterLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CameraLink, camera);
        zox_sys_o(PlayerStateDirty, dirty);
        zox_sys_o(PlayerState, state);
        zox_sys_o(CharacterLink, character);
        // Playing happily!
        if (state->value == zox_player_state_playing || state->value == zox_player_state_paused) {
            byte is_character_dead = zox_valid(character->value) && zox_getv(character->value, Dead);
            if (is_character_dead) {
                // start respawn timer!
                //  + active red overlay
                state->value = zox_player_state_respawn_begin;
                dirty->value = zox_dirty_trigger;
                if (dbg_log) {
                    zox_log("Respawn [begin] (character dead)");
                }
                // Position camera above looking at character?
                // zox_set(camera->value, Position3D, { float3_single(4) });
            }
        } else if (state->value == zox_player_state_respawning) {
            byte is_character_alive = zox_valid(character->value) && !zox_getv(character->value, Dead);
            if (is_character_alive) {
                zox_loge("Respawn [cancel] as character alive??? [%s]", zox_get_name(character->value));
                state->value = zox_player_state_play_begin;
                dirty->value = zox_dirty_trigger;
            }
        }
        // what happened HHere...? NANI?
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        // Only respawnn if playing state
        // What happens after death??
        if (state->value == zox_player_state_respawn) {
            byte is_character_dead_or_gone = !zox_valid(character->value) || zox_getv(character->value, Dead);
            if (is_character_dead_or_gone) {
                state->value = zox_player_state_play_begin;
                dirty->value = zox_dirty_trigger;
                entity game = zox_get_parent(world, e);
                entity realm = zox_getv(game, RealmLink);
                entity terrain = zox_get_child_by_id(world, realm, zox_id(Terrain));
                float3 spawned;
                character->value = game_start_player_new(world, e, realm, terrain, camera->value, &spawned, dbg_log);
                spawn_arrow3D(world, spawned, (float3) { 0, 1, 0}, 0.2f, 6, 15);
                if (dbg_log) {
                    zox_log("Player Respawned at [%fx%fx%f]", spawned.x, spawned.y, spawned.z);
                }
            }
        }
    }
} zox_sys_end(Player3RespawnSystem);
