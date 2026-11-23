/*
 * +------------------------------------------------------+
 * | Zox Module: IK (bones submodule)                     |
 * |                                                      |
 * |  Arms reach for a point: the skeleton's own bones    |
 * |                                                      |
 * +------------------------------------------------------+
 *
 *  - a shoulder bone with IKTarget (a point in the world) and IKWeight (0 off -> 1 reached) bends its arm to it:
 *    the shoulder turns the upper arm (shoulder to elbow), the elbow (the hand's parent, the forearm) turns the
 *    lower arm, two bone ik solved analytically (fun/two_bone.c), no new bones, whatever the arm is built of
 *  - the weight blends over the arm's other animation (swing, raise), so ik eases on and off
 *  - drivers write the target and weight: a swing (sys/swing.c, dat/swings.c) reaches where the head looks, XR hands
 *    (xr/fun/hands.c) put each hand on its controller
 *  - legs too (sys/leg_ik.c, a thigh's IKTarget its foot): planted feet (sys/feet.c, PlantFeet on a skeleton, XR's
 *    body) hold as the body shifts over them, stepping back under it when it's too far
 */
// its components are included with the bones' (bones/_.c), its systems read by theirs (arm_control.c: HipsShift)
#include "fun/_.c"
#include "sys/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_ik(ecs* world) {
    zox_module(ik);
    zox_components_ik(world);
    zox_systems_ik(world);
#ifdef zox_tests
    zox_tsts_ik(world);
#endif
}
