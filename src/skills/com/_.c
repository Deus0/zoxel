//zoxc_userdata(Skill);
zox_tag(Skill);
zoxc_entity(SkillLink);
zoxc_entitiesw(SkillLinks);   // Used for realm atm
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
    zoxd_tag(Skill);
    zoxd_entity(SkillLink);
    zoxd_entitiesw(SkillLinks);
    zoxd_tag(Melee);
    zoxd_tag(Aura);
    zoxd_tag(Poison);
    // zoxd_userdata(Skill);
    zoxd_float(SkillDamage);
    zoxd_float(SkillDamageMax);
    zoxd_float(SkillRange);
    zoxd_float(SkillCost);
    zoxd_byte(SkillActive);
    zoxd_entity(SkillResourceLink);
}