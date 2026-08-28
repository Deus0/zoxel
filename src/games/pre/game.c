entity spawn_prefab_game(ecs* world) {
    zox_prefab();
    zox_prefab_name("game");
    zox_add(e, Game);
    zox_setv(e, GameState, zox_game_start);
    zox_setv(e, GameStateDirty, 0);
    zox_setv(e, GameStateTime, 0);
    zox_setv(e, RealmLink, 0);
    // Remove these and just use dirty
    zox_setv(e, GameStateTarget, zox_game_start);
    zox_setv(e, LastGameState, zox_game_start);
    zox_add(e, GameStateEvent);
    return e;
}

entity spawn_game(ecs* world) {
    zox_instance(prefab_game);
    zox_name("game");
    local_game = e;
    return e;
}
