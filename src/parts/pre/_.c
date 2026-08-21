#include "attach_slot.c"
#include "body_slot.c"

void spawn_prefabs_bodys(ecs* world) {
    if (prefab_character3) {
        zox_prefab_character_add(BodySize);
        zox_prefab_character_add(BodyDirty);
        zox_prefab_character_add(CombineVox);
        zox_prefab_character_add(CombineList);
        zox_prefab_character_add(CombinePositions);
    }
    // Body is after Characters
    if (prefab_character3_skeleton) {
        entity e = prefab_character3_skeleton;
        entity body_slot = spawn_body_slot(world, e, body_anchor_core);
        zox_add(body_slot, Body);
    }
}
