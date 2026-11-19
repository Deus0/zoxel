zox_tag(Skill);
zox_tag(SkillToggleable);
zox_tag(SkillInstant);       // no cast (no warmup): it happens on the press, its swing's hit lands it; else it's cast (a castbar)
zox_tag(Skillbook);
zox_tag(Dot);
zox_tag(Melee);
zox_tag(Shoot);
zox_tag(Tentacle);           // a whip of particles from its user's body, out at its aim and back (sys/tentacle.c)
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
zoxc_float2(SkillReach);     // the distances it lands at (min, max): an npc picks its skill by it (ai/fun/skills.c)
zoxc_float2(Burn);           // its hits burn (damage a second, seconds), shoot's (pre/burn.c)
zoxc_entity(SkillUsing);     // a user's skill in progress over time (a pounce, a tentacle), one at a time
zoxc_double(SkillUsedAt);    // its start
zoxc_byte(SkillStage);       // how far along (each skill's own: in the air, its hit tried)
zoxc_float3(SkillTarget);    // where it's aimed, set as it starts (a tentacle's tip: a step aside dodges it)

void zox_components_skills(ecs* world) {
    zoxd_tag(Skill);
    zoxd_tag(SkillResource);
    zoxd_nf_link(SkillDamageStat);
    zoxd_tag(Dot);
    zoxd_tag(Poison);
    zoxd_tag(SkillToggleable);
    zoxd_tag(SkillInstant);
    zoxd_tag(Skillbook);
    zoxd_tag(Melee);
    zoxd_tag(Shoot);
    zoxd_tag(Tentacle);
    zoxd_tag(AuraDot);
    zoxd_tag(Spawner);
    zoxd_float(SkillDamage);
    zoxd_float(SkillDamageMax);
    zoxd_float(SkillRange);
    zoxd_float(SkillCost);
    zoxd_float2(SkillReach);
    zoxd_float2(Burn);
    zoxd_entity(SkillUsing);
    zoxd_double(SkillUsedAt);
    zoxd_byte(SkillStage);
    zoxd_float3(SkillTarget);
    zoxd_byte(SkillActive);
}
