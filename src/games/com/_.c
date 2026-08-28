zox_tag(Game);
zoxc_byte(GameState);
zoxc_byte(LastGameState);
zoxc_byte(GameStateTarget);
zoxc_state(GameStateDirty);
zoxc_double(GameStateTime);
zoxc_entity(GameLink);
// Passes out game + state
zoxc_listener(GameStateEvent, 16, ecs*, entity, byte);

void define_components_games(ecs* world) {
    zoxd_tag(Game);
    zoxd_byte(GameState);
    zoxd_byte(LastGameState);
    zoxd_byte(GameStateTarget);
    zoxd_state(GameStateDirty);
    zoxd_double(GameStateTime);
    zoxd_entity(GameLink);
    zoxd(GameStateEvent);
}
