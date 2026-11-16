#include "arms_in_step.c"
#include "head_camera.c"
#include "footsteps.c"

void zox_tsts_characters(ecs* world) {
    zox_tsts_arms_in_step(world);
    zox_tsts_head_camera(world);
    zox_tsts_footsteps(world);
}
