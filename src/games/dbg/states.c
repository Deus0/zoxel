extern entity dbg_player;

void test_game_end(ecs *world) {
    entity player = dbg_player;
    entity game = zox_get_link(world, player, GameLink);
    if (!zox_valid(game)) {
        return;
    }
    zox_log("> game toggling [%s]", zox_get_name(game));
    zox_geter_value(game, GameState, byte, gstate);
    if (gstate == zox_game_start) {
        zox_log("+ game starting %i", gstate);
        zox_set(game, GameStateTarget, { zox_game_load_start });
    } else if (gstate == zox_game_state_playing || gstate == zox_game_state_paused) {
        zox_log("- game ending: %i", gstate);
        zox_set(game, GameStateTarget, { zox_game_start });
    }
}
