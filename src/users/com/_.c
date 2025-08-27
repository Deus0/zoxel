zoxc_byte(Activate);
zoxc_byte(Quantity);
zoxc_byte(Warmup);
zoxc_byte(Cooldown);
zoxc_byte(Warmedup)
zoxc_byte(Cooleddown);
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
    zoxd_byte(Warmup);
    zoxd_byte(Cooldown);
    zoxd_byte(Warmedup);
    zoxd_byte(Cooleddown);
    zoxd_double(ActivateTime);
    zoxd_entity(UserLink);
    zoxd_entity(UserDataLink);
    zoxd_entity(SpawnerLink);
    zox_define_entities_component(UserLinks);
}