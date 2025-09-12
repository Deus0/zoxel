zox_tag(StatState);
zox_tag(StatRegen);
zox_tag(StatAttribute);
zox_tag(StatLevel);
zoxc_float(StatValue);
zoxc_float(StatValueMax);
zoxc_float(ExperienceValue);
zoxc_float(ExperienceMax);
zoxc_entities(DotLinks)
zoxc_userdata(Stat);
zoxf_user_prefabs(Stat, stat, "stat")
zoxc_state(StatDirty);
// special stats
zox_tag(HealthStat);
zox_tag(StatSoul);

void define_components_stats(ecs *world) {
    zoxd_tag(StatState);
    zoxd_tag(StatRegen);
    zoxd_tag(StatAttribute);
    zoxd_tag(StatLevel);
    zoxd_userdata(Stat);
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