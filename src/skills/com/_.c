zox_tag(Skill);
zox_tag(SkillToggleable);
zox_tag(Skillbook);
zox_tag(Dot);
zox_tag(Melee);
zox_tag(Shoot);
zox_tag(AuraDot);
zox_tag(Poison);
zox_tag(Spawner);
zox_tag(SkillResource);
zox_tag(SkillDamageStat);    // realm stat added to skill damage, ie physical damage
zoxc_byte(SkillActive);
zoxc_float(SkillDamage);
zoxc_float(SkillDamageMax);
zoxc_float(SkillRange);
zoxc_float(SkillCost);

void zox_components_skills(ecs* world) {
    zoxd_tag(Skill);
    zoxd_tag(SkillResource);
    zoxd_nf_tag(SkillDamageStat);
    zoxd_tag(Dot);
    zoxd_tag(Poison);
    zoxd_tag(SkillToggleable);
    zoxd_tag(Skillbook);
    zoxd_tag(Melee);
    zoxd_tag(Shoot);
    zoxd_tag(AuraDot);
    zoxd_tag(Spawner);
    zoxd_float(SkillDamage);
    zoxd_float(SkillDamageMax);
    zoxd_float(SkillRange);
    zoxd_float(SkillCost);
    zoxd_byte(SkillActive);
}
