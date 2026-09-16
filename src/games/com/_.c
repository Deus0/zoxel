zox_tag(Game);
zox_tag(GameLink);
zoxc_byte(GameState);
zoxc_byte(LastGameState);
zoxc_byte(GameStateTarget);
zoxc_state(GameStateDirty);
zoxc_double(GameStateTime);
// Passes out game + state
zoxc_listener(GameStateEvent, 16, ecs*, entity, byte);

entity get_linked_game(ecs* world, entity e) {
    return zox_get_link(world, e, GameLink);
}

void zox_link_game(ecs* world, entity e, entity game) {
    zox_link(world, e, GameLink, game);
}

void define_components_games(ecs* world) {
    zoxd_tag(Game);
    zoxd_nf_tag(GameLink);
    zoxd_byte(GameState);
    zoxd_byte(LastGameState);
    zoxd_byte(GameStateTarget);
    zoxd_state(GameStateDirty);
    zoxd_double(GameStateTime);
    zoxd(GameStateEvent);
}
