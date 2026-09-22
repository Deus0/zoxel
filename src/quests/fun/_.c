#include "label.c"

entity get_user_quest(
    ecs* world,
    entity quest_taker,
    entity quest)
{
    entity user_quest = 0;
    iter it2 = zox_children(world, quest_taker);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity child = it2.entities[j];
            if (!zox_has(child, Quest)) {
                continue;
            }
            entity prefab = zox_get_prefab(world, child);
            if (prefab == quest) {
                user_quest = child;
                break;
            }
        }
    }
    return user_quest;
}
