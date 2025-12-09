#include "state.c"
#include "level.c"
#include "regen.c"
#include "attribute.c"
#include "stat.c"

void spawn_prefabs_stats(ecs *world) {
    entity prefab_stat = spawn_prefab_stat(world);
    zox_set(prefab_stat, StatDirty, { zox_dirty_none });
    if (prefab_realm) {
        zox_prefab_set(prefab_realm, StatLinks, { 0 });
    }
    zox_prefab_character_set(StatLinks, { 0 });
    zox_prefab_character_set(DotLinks, { 0 });
}