#if !defined(zoxm_skills) && defined(zoxm_users)
#define zoxm_skills

#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "ui/_.c"
#include "tst/aura.c"

zox_begin_module(Skills)
    define_components_skills(world);
    define_systems_skills(world);
    add_hook_spawned_character3D(&spawn_character_skills);
    add_hook_spawn_prefabs(spawn_prefabs_skills);
    zox_import_module(UISkills);
zox_end_module(Skills)

#endif