zoxc_byte(Activate);
zoxc_byte(Quantity);
zoxc_byte(WarmupState);
zoxc_byte(CooldownState);
zoxc_double(Warmup);
zoxc_double(Cooldown);
zoxc_double(WarmupTime);
zoxc_double(CooldownTime);
zoxc_double(ActivateTime);
zoxc_entity(UserLink);
zoxc_entity(UserDataLink);
zoxc_entity(SpawnerLink);
zoxc_entities(UserLinks)

// TODO: Warmup and Cooldown bytes -> to float, we have a max cooldown times or something
// TODO: Make state systems for cooldown warmup and use this for activating skills etc

void define_components_users(ecs* world) {
    zoxd_byte(Activate);
    zoxd_byte(Quantity);
    zoxd_byte(WarmupState);
    zoxd_byte(CooldownState);
    zoxd_double(WarmupTime);
    zoxd_double(CooldownTime);
    zoxd_double(ActivateTime);
    zoxd_double(Warmup);
    zoxd_double(Cooldown);
    zoxd_entity(UserLink);
    zoxd_entity(UserDataLink);
    zoxd_entity(SpawnerLink);
    zox_define_entities_component(UserLinks);
}