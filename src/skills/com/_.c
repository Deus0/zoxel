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
zox_tag(Burn);               // a short dot from a skill with SkillBurn (pre/burn.c)
zox_tag(Spawner);
zox_tag(SkillResource);
zox_tag(SkillDamageStat);    // realm stat added to skill damage, ie physical damage
zoxc_byte(SkillActive);
zoxc_float(SkillDamage);
zoxc_float(SkillDamageMax);
zoxc_float(SkillRange);
zoxc_float(SkillCost);
zoxc_float2(SkillReach);
zoxc_float2(SkillBurn);      // its hits burn (damage a second, seconds), shoot's (pre/burn.c)
zoxc_double(PounceStart);    // a boneless user's melee (a slime's): its squash's start (sys/pounce.c)
zoxc_byte(PounceStage);      // 0 squashing, 1 in the air, 2 its hit landed
zoxc_entity(PounceSkill);    // the melee skill it pounces with
zoxc_double(TentacleStart);  // a user's tentacle out (sys/tentacle.c): its start
zoxc_float3(TentacleTip);    // where it reaches, set as it starts (it can be dodged)
zoxc_entity(TentacleSkill);  // the skill it whips with
zoxc_byte(TentacleHit);      // its hit tried, once
zoxc_float(TentacleBit);     // a particle of a tentacle (its user's child): its place along it, 0 its body, 1 its tip     // the distances it lands at (min, max): an npc picks its skill by it (ai/fun/skills.c)

void zox_components_skills(ecs* world) {
    zoxd_tag(Skill);
    zoxd_tag(SkillResource);
    zoxd_nf_link(SkillDamageStat);
    zoxd_tag(Dot);
    zoxd_tag(Poison);
    zoxd_tag(Burn);
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
    zoxd_float2(SkillBurn);
    zoxd_double(PounceStart);
    zoxd_byte(PounceStage);
    zoxd_entity(PounceSkill);
    zoxd_double(TentacleStart);
    zoxd_float3(TentacleTip);
    zoxd_entity(TentacleSkill);
    zoxd_byte(TentacleHit);
    zoxd_float(TentacleBit);
    zoxd_byte(SkillActive);
}
