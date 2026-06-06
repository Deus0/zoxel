zox_tag(Player);
zox_tag(PlayerCharacter);
zox_tag(Player2);
zox_tag(Player3);
zoxc_byte(PlayerState);
zoxc_state(PlayerStateDirty);
zoxc_double(PlayerStateTimer);
zoxc_double(PlayerRespawn);
zoxc_entity(PlayerLink);
zoxc_arrayd(PlayerLinks, entity)
zoxc_entity(PlayerPauseEvent);

void define_components_players(ecs* world) {
    zoxd_tag(Player);
    zoxd_tag(Player2);
    zoxd_tag(Player3);
    zoxd_tag(PlayerCharacter);
    zoxd_byte(PlayerState);
    zoxd_state(PlayerStateDirty);
    zoxd_double(PlayerStateTimer);
    zoxd_double(PlayerRespawn);
    zoxd_entity(PlayerLink);
    zoxd_arrayd(PlayerLinks);
    zoxd_entity(PlayerPauseEvent);
}
