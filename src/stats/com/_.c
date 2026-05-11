//zoxc_userdata(Stat);
zox_tag(Stat);
zoxc_entity(StatLink);
zoxc_entities(StatLinks);   // Used for realm atm
// Types
zox_tag(StatBase);
zox_tag(StatState);
zox_tag(StatRegen);
zox_tag(StatAttribute);
zox_tag(StatLevel);
// Data
zoxc_float(StatValue);
zoxc_float(StatValueMax);
zoxc_float(ExperienceValue);
zoxc_float(ExperienceMax);
zoxc_entities(DotLinks)
zoxc_state(StatDirty);
// special stats
zox_tag(HealthStat);
zox_tag(StatSoul);

void define_components_stats(ecs *world) {
    zoxd_tag(Stat);
    zoxd_entity(StatLink);
    zoxd_entities(StatLinks);
    // Types
    zoxd_tag(StatBase);
    zoxd_tag(StatState);
    zoxd_tag(StatRegen);
    zoxd_tag(StatAttribute);
    zoxd_tag(StatLevel);
    // Data
    zoxd_state(StatDirty);
    zoxd_float(StatValue);
    zoxd_float(StatValueMax);
    zoxd_float(ExperienceValue);
    zoxd_float(ExperienceMax);
    zoxd_entities(DotLinks);
    // special stats
    zoxd_tag(HealthStat);
    zoxd_tag(StatSoul);
}
