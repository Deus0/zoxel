entity prefab_skill;
entity prefab_debuff;
entity prefab_poison;
entity prefab_burn;
entity prefab_skill_melee;
entity prefab_skill_shoot;
entity prefab_skill_tentacle;
#include "skill.c"
#include "poison.c"
#include "burn.c"
#include "melee.c"
#include "shoot.c"
#include "tentacle.c"

void spawn_prefabs_skills(ecs* world) {
    prefab_skill = spawn_prefab_skill(world);
    prefab_poison = spawn_prefab_poison(world);
    prefab_burn = spawn_prefab_burn(world);
    prefab_skill_melee = spawn_prefab_melee(
        world,
        prefab_skill);
    prefab_skill_shoot = spawn_prefab_shoot(
        world,
        prefab_skill);
    prefab_skill_tentacle = spawn_prefab_tentacle(world, prefab_skill);
}
