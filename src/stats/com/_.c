zox_tag(Stat);
zox_tag(StatLink);
// Types
zox_tag(StatBase);
zox_tag(StatState);
zox_tag(StatRegen);
zox_tag(StatAttribute);
zox_tag(StatLevel);
zox_tag(StatCurrency);   // points spent on things, ie stat point
zox_tag(StatDisabled);  // stat stops processing, ie regen on death
// Data
zoxc_float(StatValue);
zoxc_float(StatValueMax);
zoxc_float(ExperienceValue);
zoxc_float(ExperienceMax);
zoxc_float(StatLevelReward);    // currency gained per level up
// special stats
zox_tag(StatHealth);
zox_tag(StatSoul);
zox_tag(StatPoint);     // currency spent on attributes
zox_tag(SkillPoint);    // currency spent on skills
zox_tag(SpawnNameLabel);


void zox_components_stats(ecs *world) {
    zoxd_tag(Stat);
    zoxd_nf_tag(StatLink);
    zoxd_tag(SpawnNameLabel);
    // Types
    zoxd_tag(StatBase);
    zoxd_tag(StatState);
    zoxd_tag(StatRegen);
    zoxd_tag(StatAttribute);
    zoxd_tag(StatLevel);
    zoxd_tag(StatDisabled);
    zoxd_tag(StatCurrency);
    // Data
    zoxd_float(StatValue);
    zoxd_float(StatValueMax);
    zoxd_float(ExperienceValue);
    zoxd_float(ExperienceMax);
    zoxd_float(StatLevelReward);
    // special stats
    zoxd_tag(StatHealth);
    zoxd_tag(StatSoul);
    zoxd_tag(StatPoint);
    zoxd_tag(SkillPoint);
}
