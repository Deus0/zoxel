zox_tag(Game);
zoxc_byte(GameState);
zoxc_byte(GameStateTarget);
zoxc_state(GameStateDirty);
zoxc_double(GameStateTime);
zoxc_entity(GameLink);

void define_components_games(ecs* world) {
    zoxd_tag(Game);
    zoxd_byte(GameState);
    zoxd_byte(GameStateTarget);
    zoxd_state(GameStateDirty);
    zoxd_double(GameStateTime);
    zoxd_entity(GameLink);
}