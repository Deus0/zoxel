zox_tag(Player);
zox_tag(PlayerCharacter);
zox_tag(Player2);
zox_tag(Player3);
zoxc_byte(PlayerState);
zoxc_state(PlayerStateDirty);
zoxc_byte(PlayerStateTarget);  // the next state, set from observers (the Dirty is on there), player_state_system applies and removes it
zoxc_double(PlayerStateTimer);
zoxc_double(PlayerRespawn);
zox_tag(PlayerLinks);      // links to many players, i.e. from a game
zox_tag(PlayerLink);

void zox_components_players(ecs* world) {
    zoxd_tag(Player);
    zoxd_tag(Player2);
    zoxd_tag(Player3);
    zoxd_tag(PlayerCharacter);
    zoxd_byte(PlayerState);
    zoxd_state(PlayerStateDirty);
    zoxd_byte(PlayerStateTarget);
    zoxd_double(PlayerStateTimer);
    zoxd_double(PlayerRespawn);
    zoxd_nf_links(PlayerLinks);
    zoxd_nf_link(PlayerLink);
}
