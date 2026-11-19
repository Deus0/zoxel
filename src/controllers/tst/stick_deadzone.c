// tst: stick_deadzone, a gamepad stick's round dead zone (inputs/fun/deadzone_util.c), no world
//  - an old xbox controller's sticks rest about a fifth off the middle: inside the dead zone they're still
//  - past it: scaled from 0 (no jump), full at the edge, the way kept
//  - run: bash bsh/tst.sh stick_deadzone
byte zox_tst_stick_deadzone_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_stick_deadzone_check(ecs* world) {
    (void) world;
    const float d = 0.2f;
    const float2 rest = stick_deadzone((float2) { 0.15f, -0.12f }, d);      // a drifting stick at rest, 0.19 off
    const float2 edge = stick_deadzone((float2) { 1, 0 }, d);
    const float2 half = stick_deadzone((float2) { 0, 0.6f }, d);
    const float2 just = stick_deadzone((float2) { 0.21f, 0 }, d);
    const float2 diagonal = stick_deadzone((float2) { 0.6f, 0.6f }, d);
    zox_tst_check(rest.x == 0 && rest.y == 0, "a stick resting 0.19 off moved [%f %f]", rest.x, rest.y);
    zox_tst_check(fabsf(edge.x - 1) < 0.001f && edge.y == 0, "full right gave [%f %f]", edge.x, edge.y);
    zox_tst_check(half.x == 0 && fabsf(half.y - 0.5f) < 0.001f, "0.6 up gave [%f], expected 0.5", half.y);
    zox_tst_check(just.x > 0 && just.x < 0.02f, "just past the dead zone gave [%f], it should start near 0", just.x);
    zox_tst_check(fabsf(diagonal.x - diagonal.y) < 0.001f && diagonal.x > 0, "a diagonal turned [%f %f]", diagonal.x, diagonal.y);
}
