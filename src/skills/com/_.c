zoxc_userdata(Skill);
zox_tag(Melee);
zox_tag(Aura);
zox_tag(Poison);
zoxc_byte(SkillActive);
zoxc_float(SkillDamage);
zoxc_float(SkillDamageMax);
zoxc_float(SkillRange);
zoxc_float(SkillCost);
zoxc_entity(SkillResourceLink);

void define_components_skills(ecs* world) {
    zoxd_tag(Melee);
    zoxd_tag(Aura);
    zoxd_tag(Poison);
    zoxd_userdata(Skill);
    zox_define_component_float(SkillDamage);
    zox_define_component_float(SkillDamageMax);
    zox_define_component_float(SkillRange);
    zox_define_component_float(SkillCost);
    zoxd_byte(SkillActive);
    zoxd_entity(SkillResourceLink);
}