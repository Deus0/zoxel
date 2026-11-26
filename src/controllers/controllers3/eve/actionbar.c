void player_state_actionbar(
    ecs* world,
    entity player,
    byte state)
{
    byte dbg_log = 0;
    byte is_spawn = state == zox_player_state_play_begin;
    byte is_destroy = state == zox_player_state_respawn_begin;
    if (!is_destroy && !is_spawn) {
        return;
    }
    entity canvas = zox_get_link(world, player, CanvasLink);
    if (!zox_valid(canvas)) {
        return;
    }
    entity ui = zox_get_child_by_id(
        world,
        canvas,
        zox_id(MenuActions));
    // a player without a character, or a character without an actionbar (zeland's), has no actions
    const entity character = zox_get_link(world, player, CharacterLink);
    if (is_spawn && !zox_valid(ui) && zox_valid(character) && zox_get_child_by_id(world, character, zox_id(Actionbar))) {
        spawn_menu_actions(world, player);
        if (dbg_log) {
            zox_log("Spawned [MenuActions] for [%s]",
                zox_getn(player));
        }
    } else if (is_destroy && zox_valid(ui)) {
        if (dbg_log) {
            zox_log("Destroying [MenuActions] [%s] for [%s]",
                zox_getn(ui),
                zox_getn(player));
        }
        zox_delete(ui);
    }
}


