void player_state_minimaps(ecs* world, entity player, byte state) {
    byte dbg_log = 0;
    byte is_spawn = state == zox_player_state_play_begin;
    byte is_destroy =
        state == zox_player_state_pause_begin ||
        state == zox_player_state_respawn_begin;
    if (!is_destroy && !is_spawn) {
        return;
    }
    entity canvas = zox_get_link(world, player, Canvas);
    if (!zox_valid(canvas)) {
        // zox_logw("Canvas is missing from Player [PlayerUIGamePauseSystem]");
        return;
    }
    entity minimap = zox_get_child_by_id(world, canvas, zox_id(Minimap));
    if (is_spawn && !zox_valid(minimap)) {
        entity game = zox_get_parent(world, player);
        if (!zox_valid(game)) {
            zox_loge("Player [%s] has invalid Game.",
                zox_getn(player));
            return;
        }
        entity realm = zox_get_link(world, game, RealmLink);
        if (!zox_valid(realm)) {
            zox_loge("Game [%s] has invalid Realm.",
                zox_getn(game));
            return;
        }
        entity terrain = zox_get_link(world, realm, TerrainLink);
        spawn_minimap(
            world,
            canvas,
            player,
            terrain);
        if (dbg_log) {
            zox_log("Spawned Player UI [%s]: [Minimap]",
                zox_getn(player));
        }
    } else if (is_destroy && zox_valid(minimap)) {
        if (dbg_log) {
            zox_log("Destroying Player UI [%s]: [Minimap]:[%s]",
                zox_getn(player),
                zox_getn(minimap));
        }
        zox_delete(minimap);
    }
}

