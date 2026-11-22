#include "gizmo_owner.c"
#include "held_item.c"
#include "held_view.c"
#include "eat_swing.c"
#include "place_hit.c"
#include "castbar.c"
#include "bone_rebuild.c"
#include "mirror_hat.c"

#include "placement_box.c"

void zox_tsts_actions(ecs* world) {
    zox_tsts_gizmo_owner(world);
    zox_tsts_placement_box(world);
    zox_tsts_held_item(world);
    zox_tst_add("held_view", zox_tst_held_view_ready, zox_tst_held_view_check, 120);
    zox_tst_set_setup("held_view", zox_tst_level_setup, 0);
    zox_tsts_eat_swing(world);
    zox_tsts_place_hit(world);
    zox_tsts_castbar(world);
    zox_tsts_bone_rebuild(world);
    zox_tst_add("mirror_hat", zox_tst_mirror_hat_ready, zox_tst_mirror_hat_check, 60);
}
