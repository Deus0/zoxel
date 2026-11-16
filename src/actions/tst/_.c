#include "gizmo_owner.c"
#include "held_item.c"
#include "bone_rebuild.c"

#include "placement_box.c"

void zox_tsts_actions(ecs* world) {
    zox_tsts_gizmo_owner(world);
    zox_tsts_placement_box(world);
    zox_tsts_held_item(world);
    zox_tsts_bone_rebuild(world);
}
