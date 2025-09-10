zoxc_byte(CanActivate);     // stats resource will validate this
zoxc_double(WarmupAt);
zoxc_double(CooldownAt);
zoxc_double(WarmupTime);
zoxc_double(CooldownTime);
zoxc_double(ActivateTime);
zoxc_byte(Quantity);
zoxc_entity(UserLink);
zoxc_entity(UserDataLink);
zoxc_entity(SpawnerLink);
zoxc_entities(UserLinks);
zoxc_state(ActivateBegin);
zoxc_state(WarmupState);
zoxc_state(Activate);
zoxc_state(CooldownState);
zoxc_state(UserDataDirty);

void define_components_users(ecs* world) {
    zoxd_byte(CanActivate);
    zoxd_double(WarmupTime);
    zoxd_double(CooldownTime);
    zoxd_double(ActivateTime);
    zoxd_double(WarmupAt);
    zoxd_double(CooldownAt);
    zoxd_byte(Quantity);
    zoxd_entity(UserLink);
    zoxd_entity(UserDataLink);
    zoxd_entity(SpawnerLink);
    zoxd_entities(UserLinks);
    zoxd_state(ActivateBegin);
    zoxd_state(Activate);
    zoxd_state(WarmupState);
    zoxd_state(CooldownState);
    zoxd_state(UserDataDirty);
}