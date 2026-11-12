zox_tag(Stat);
zox_tag(StatLink);
zox_tag(StatScaleLink);      // realm stat -> realm attribute that boosts it
zox_tag(StatDefenceLink);    // realm damage stat -> realm stat that reduces it
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
zoxc_float(StatScaleAmount);    // bonus per point of the StatScaleLink attribute
zoxc_float(StatBonus);          // attribute bonus currently applied to a users stat
// special stats
zox_tag(StatHealth);
zox_tag(StatSoul);
zox_tag(StatPoint);     // currency spent on attributes
zox_tag(SkillPoint);    // currency spent on skills
zox_tag(StatDamage);    // base stat added to skill damage, ie physical damage
zox_tag(StatDefence);   // base stat that reduces damage, ie physical defence
zox_tag(SpawnNameLabel);   // has a name label near players (character_name_labels_system)
zox_tag(NameLabelLink);


void zox_components_stats(ecs *world) {
    zoxd_tag(Stat);
    zoxd_nf_link(StatLink);
    zoxd_nf_link(StatScaleLink);
    zoxd_nf_link(StatDefenceLink);
    zoxd_tag(SpawnNameLabel);
    zoxd_nf_link(NameLabelLink);
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
    zoxd_float(StatScaleAmount);
    zoxd_float(StatBonus);
    // special stats
    zoxd_tag(StatHealth);
    zoxd_tag(StatSoul);
    zoxd_tag(StatPoint);
    zoxd_tag(SkillPoint);
    zoxd_tag(StatDamage);
    zoxd_tag(StatDefence);
}
