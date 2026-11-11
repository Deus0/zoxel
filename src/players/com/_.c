zox_tag(Player);
zox_tag(PlayerCharacter);
zox_tag(Player2);
zox_tag(Player3);
zoxc_byte(PlayerState);
zoxc_state(PlayerStateDirty);
zoxc_double(PlayerStateTimer);
zoxc_double(PlayerRespawn);
zox_tag(PlayerLinks);      // links to many players, i.e. from a game
zoxc_listener(PlayerStateEvent, 32, ecs*, entity, byte);
zox_tag(PlayerLink);

void zox_components_players(ecs* world) {
    zoxd_tag(Player);
    zoxd_tag(Player2);
    zoxd_tag(Player3);
    zoxd_tag(PlayerCharacter);
    zoxd_byte(PlayerState);
    zoxd_state(PlayerStateDirty);
    zoxd_double(PlayerStateTimer);
    zoxd_double(PlayerRespawn);
    zoxd_nf_links(PlayerLinks);
    zoxd(PlayerStateEvent);
    zoxd_nf_link(PlayerLink);
}
