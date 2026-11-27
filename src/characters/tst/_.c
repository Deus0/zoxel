#include "character.c"
#include "arms_in_step.c"
#include "arm_reach.c"
#include "punch_motion.c"
#include "swing_motion.c"
#include "place_glance.c"
#include "guard_steady.c"
#include "guard_pitch.c"
#include "realm_pause.c"
#include "feet_planted.c"
#include "head_camera.c"
#include "footsteps.c"
#include "swim.c"
#include "doorway.c"
#include "character_light.c"
#include "step_gap.c"
#include "poly_character.c"

void zox_tsts_characters(ecs* world) {
    zox_tsts_arms_in_step(world);
    zox_tsts_arm_reach(world);
    zox_tsts_punch_motion(world);
    zox_tsts_swing_motion(world);
    zox_tsts_place_glance(world);
    zox_tsts_guard_steady(world);
    zox_tsts_guard_pitch(world);
    zox_tsts_realm_pause(world);
    zox_tsts_feet_planted(world);
    zox_tsts_head_camera(world);
    zox_tsts_footsteps(world);
    zox_tsts_swim(world);
    zox_tsts_doorway(world);
    zox_tsts_character_light(world);
    zox_tsts_step_gap(world);
    zox_tsts_poly_character(world);
}
