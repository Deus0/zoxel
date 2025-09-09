entity spawn_prefab_game(ecs* world) {
    zox_prefab();
    zox_prefab_name("game");
    zox_add_tag(e, Game);
    zox_prefab_set(e, GameState, { zox_game_start });
    zox_prefab_set(e, GameStateTarget, { zox_game_start });
    zox_prefab_set(e, GameStateDirty, { zox_dirty_none });
    zox_prefab_set(e, GameStateTime, { 0 });
    zox_prefab_set(e, RealmLink, { 0 });
    return e;
}

entity spawn_game(ecs* world, const entity realm) {
    zox_instance(prefab_game);
    zox_name("game");
    if (realm) {
        zox_set(e, RealmLink, { realm });
        zox_set(realm, GameLink, { e });
    }
    return e;
}
