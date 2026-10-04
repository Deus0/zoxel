void player_state_quest_tracker(
    ecs* world,
    entity player,
    byte state)
{
    byte dbg_log = 0;
    byte is_spawn = state == zox_player_state_play_begin;
    byte is_destroy =
        state == zox_player_state_pause_begin ||
        state == zox_player_state_respawn_begin;
    if (!is_destroy && !is_spawn) {
        return;
    }
    entity canvas = zox_get_link(world, player, CanvasLink);
    if (!zox_valid(canvas)) {
        // zox_logw("Canvas is missing from Player [PlayerUIGamePauseSystem]");
        return;
    }
    entity tracker = zox_get_child_by_id(
        world,
        canvas,
        zox_id(QuestTracker));
    if (is_spawn && !zox_valid(tracker)) {
        spawn_quest_tracker(
            world,
            canvas,
            player);
        if (dbg_log) {
            zox_log("Spawned Player UI [%s]: [quest_tracker]",
                zox_getn(player));
        }
    } else if (is_destroy && zox_valid(tracker)) {
        if (dbg_log) {
            zox_log("Destroying Player UI [%s]: [quest_tracker]:[%s]",
                zox_getn(player),
                zox_getn(tracker));
        }
        zox_delete(tracker);
    }
}

