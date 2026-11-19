#include "clothes.c"
#include "body_views.c"
#include "body_poses.c"
#include "ik_poses.c"
#include "punch_views.c"
#include "walk_views.c"

void zox_tsts_equipment(ecs* world) {
    zox_tsts_clothes(world);
    zox_tsts_body_views(world);
    zox_tsts_body_poses(world);
    zox_tsts_ik_poses(world);
    zox_tsts_punch_views(world);
    zox_tsts_walk_views(world);
}
