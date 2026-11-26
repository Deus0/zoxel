zox_tag(Game);
zox_tag(GameLink);
zoxc_byte(GameState);
zoxc_byte(LastGameState);
zoxc_byte(GameStateTarget);
zoxc_byte(GameStateWait);   // frames left before the next transition (game_state_system)
zoxc_double(GameStateTime);

entity get_linked_game(ecs* world, entity e) {
    return zox_get_link(world, e, GameLink);
}

void zox_link_game(ecs* world, entity e, entity game) {
    zox_link(world, e, GameLink, game);
}

void zox_components_games(ecs* world) {
    zoxd_tag(Game);
    zoxd_nf_link(GameLink);
    zoxd_byte(GameState);
    zoxd_byte(LastGameState);
    zoxd_byte(GameStateTarget);
    zoxd_byte(GameStateWait);
    zoxd_double(GameStateTime);
}
