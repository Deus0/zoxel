entity spawn_prefab_player(ecs *world) {
    zox_prefab();
    zox_prefab_name("player");
    zox_add_tag(e, Player);
    zox_prefab_set(e, PlayerState, { zox_player_state_main_menu });
    zox_prefab_set(e, PlayerStateDirty, { 0 });
    zox_prefab_set(e, PlayerRespawn, { 0 });
    zox_prefab_set(e, PlayerPauseEvent, { 0 });
    zox_prefab_set(e, GameLink, { 0 });
    // zox_prefab_set(e, RealmLink, { 0 });
    return e;
}
