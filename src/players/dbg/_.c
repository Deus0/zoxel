uint debug_ui_player(ecs *world, entity e, char *buffer, uint size, uint index) {
    if (!e) {
        index += snprintf(buffer + index, size - index, "Invalid Player\n");
        return index;
    }
    byte player_state = zox_getv(e, PlayerState);
    index += snprintf(buffer + index, size - index, "Player [%s]\n", zox_get_name(e));
    index += snprintf(buffer + index, size - index, " - [%i]\n", player_state);
    entity game = zox_get_parent(world, e);
    if (!zox_valid(game)) {
        index += snprintf(buffer + index, size - index, "Invalid Game\n");
        return index;
    }
    byte game_state = zox_getv(game, GameState);
    index += snprintf(buffer + index, size - index, "Game [%s]\n", zox_get_name(game));
    index += snprintf(buffer + index, size - index, " - [%i]\n", game_state);
    return index;
}
