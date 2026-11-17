#include "character.c"
#include "arms_in_step.c"
#include "arm_reach.c"
#include "punch_motion.c"
#include "guard_steady.c"
#include "head_camera.c"
#include "footsteps.c"

void zox_tsts_characters(ecs* world) {
    zox_tsts_arms_in_step(world);
    zox_tsts_arm_reach(world);
    zox_tsts_punch_motion(world);
    zox_tsts_guard_steady(world);
    zox_tsts_head_camera(world);
    zox_tsts_footsteps(world);
}
