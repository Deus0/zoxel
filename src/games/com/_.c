zox_tag(Game);
zoxc_byte(GameState);
zoxc_state(GameStateDirty);
zoxc_byte(GameStateTarget);
zoxc_double(GameStateTime);
zoxc_entity(GameLink);

void define_components_games(ecs* world) {
    zoxd_tag(Game);
    zoxd_byte(GameState);
    zoxd_state(GameStateDirty);
    zoxd_byte(GameStateTarget);
    zoxd_double(GameStateTime);
    zoxd_entity(GameLink);
}
