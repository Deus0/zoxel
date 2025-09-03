entity prefab_skill_melee;
entity prefab_skill_aura;
entity prefab_debuff;
zoxf_user_prefabs(Skill, skill, "skill")
entity prefab_aura_poison;
entity prefab_poison;

#include "melee.c"
#include "aura.c"
#include "poison.c"

void spawn_prefabs_skills(ecs *world) {
    // skill prefab
    prefab_skill = spawn_prefab_skill(world);
    zox_prefab_set(prefab_skill, SkillActive, { 0 });
    zox_prefab_set(prefab_skill, Color, { color_black });
    zox_prefab_addc_user_timings(world, prefab_skill);

    prefab_skill_melee = spawn_prefab_melee(world, prefab_skill);
    prefab_skill_aura = spawn_prefab_aura(world, prefab_skill);
    prefab_poison = spawn_prefab_poison(world);

    // link to core
    if (prefab_realm) {
        zox_prefab_add(prefab_realm, SkillLinks);
    }
    if (prefab_character3) {
        zox_prefab_character_add(SkillLinks);
    }
}
