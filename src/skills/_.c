/*
 * +-------------------------------------------------------+
 * | Zox Module: Skills                                    |
 * |                                                       |
 * |  Abilities - Effects - Cooldowns - Skill Trees        |
 * |                                                       |
 * +-------------------------------------------------------+
 */
#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "ui/_.c"
#include "auras/_.c"
#include "tst/_.c"

void import_skills(ecs* world) {
    zox_module(skills);
    zox_components_skills(world);
    zox_systems_skills(world);
    add_hook_spawn_prefabs(spawn_prefabs_skills);
    // add_hook_spawn_prefabs(zox_events_skills);
    zox_import_module(UISkills);
    zox_import_module(Auras);
}
