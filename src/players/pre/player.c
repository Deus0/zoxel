entity spawn_prefab_player(ecs *world) {
    zox_prefab();
    zox_prefab_name("player");
    zox_add(e, Player);
    zox_setv(e, PlayerState, zox_player_state_main_menu);
    zox_setv(e, PlayerStateDirty, 0);
    zox_setv(e, PlayerStateTimer, 0);
    zox_setv(e, PlayerRespawn, 0);
    zox_setv(e, PlayerPauseEvent, 0);
    zox_setv(e, GameLink, 0);
    zox_add(e, PlayerStateEvent);
    return e;
}
