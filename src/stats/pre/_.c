#include "stat.c"
entity prefab_stat;
entity prefab_stat_base;
entity prefab_stat_state;
entity prefab_stat_regen;
entity prefab_stat_attribute;
entity prefab_stat_level;

void spawn_prefabs_stats(ecs* world) {
    prefab_stat = spawn_prefab_stat(world);
    // Base
    {
        entity e = zox_prefab_from_parent(world, prefab_stat);
        zox_add(e, StatBase);
        prefab_stat_base = e;
    }
    // States + Regen
    {
        entity e = zox_prefab_from_parent(world, prefab_stat);
        zox_add(e, StatState);
        zox_set(e, StatValueMax, { 0 });
        prefab_stat_state = e;
    }
    // Regens
    {
        entity e = zox_prefab_from_parent(world, prefab_stat);
        zox_add(e, StatRegen);
        prefab_stat_regen = e;
    }
    // Levels
    {
        entity e = zox_prefab_from_parent(world, prefab_stat);
        zox_add(e, StatLevel);
        zox_prefab_set(e, ExperienceValue, { 0 });
        zox_prefab_set(e, ExperienceMax, { zox_experience_max_start });
        prefab_stat_level = e;
    }
    // Attributes
    {
        entity e = zox_prefab_from_parent(world, prefab_stat);
        zox_add(e, StatAttribute);
        prefab_stat_attribute = e;
    }
    zox_prefab_character_add(SpawnNameLabel);
}
