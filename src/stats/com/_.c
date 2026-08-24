zox_tag(Stat);
zox_tag(RealmStat);
zoxc_entity(StatLink);
zoxc_entitiesw(StatLinks);   // Used for realm atm
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
zoxc_state(StatDirty);
// special stats
zox_tag(StatHealth);
zox_tag(StatSoul);
zox_tag(SpawnNameLabel);


void zox_components_stats(ecs *world) {
    zoxd_tag(Stat);
    zoxd_tag(RealmStat);
    zoxd_tag(SpawnNameLabel);
    zoxd_entity(StatLink);
    zoxd_entitiesw(StatLinks);
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
    // special stats
    zoxd_tag(StatHealth);
    zoxd_tag(StatSoul);
}
