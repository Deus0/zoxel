entity prefab_action;

#include "action.c"

void spawn_prefabs_actions(ecs *world) {
    spawn_prefab_action(world);
    if (zox_valid(prefab_character3)) {
        zox_prefab_character_set(ActionIndex, { 0 });
        zox_prefab_character_set(ActionLinks, { 0 });
    }
}