entity prefab_skill;
entity prefab_skill_melee;
entity prefab_skill_aura;
entity prefab_debuff;
entity prefab_aura_poison;
entity prefab_poison;
#include "skill.c"
#include "melee.c"
#include "aura.c"
#include "poison.c"

void spawn_prefabs_skills(ecs* world) {
    prefab_skill = spawn_prefab_skill(world);
    prefab_skill_melee = spawn_prefab_melee(world, prefab_skill);
    prefab_skill_aura = spawn_prefab_aura(world, prefab_skill);
    prefab_poison = spawn_prefab_poison(world);
    // Link to Prior Modules
    if (prefab_realm) {
        zox_prefab_add(prefab_realm, SkillLinks);
    }
    if (prefab_character3) {
        zox_prefab_character_add(SkillLinks);
    }
}
