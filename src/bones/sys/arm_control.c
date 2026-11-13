// Tells the limbs to swing or not, and keeps the skeleton's swing timing, shared by all its limbs
//  - the skeleton's SwingPhase and SwingBlend advance here once, every limb reads them (arm_swing.c), so
//    both arms and legs stay in step (each limb kept its own phase, a raised arm fell out of step)
//  - phase accumulates, so changing speed never jumps the angle, SwingBlend eases idle <-> walking
// TODO: Only signal the bones when WalkStateDirty
#define swing_idle_speed 0.1f   // half cycles per second
#define swing_walk_speed 1.8f
#define swing_blend_rate 5.0f   // how fast idle <-> walk eases
#define swing_max_delta 0.1f

void arm_control_system(iter* it) {
    zox_sys_on_begin();
    const float delta = zox_delta_time < swing_max_delta ? zox_delta_time : swing_max_delta;
    const float blend_t = 1.0f - expf(-swing_blend_rate * delta);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(WalkState);
    zox_sys_out(SwingPhase);
    zox_sys_out(SwingBlend);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(WalkState, state);
        zox_sys_o(SwingPhase, phase);
        zox_sys_o(SwingBlend, blend);
        // the skeleton's timing
        blend->value += ((state->value ? 1.0f : 0.0f) - blend->value) * blend_t;
        phase->value += M_PI * (swing_idle_speed + (swing_walk_speed - swing_idle_speed) * blend->value) * delta;
        if (phase->value > 2 * M_PI) {
            phase->value -= 2 * M_PI;
        }
        entity bones[zox_children_capacity];
        uint length = zox_get_children_by_id_recursive(world, e, bones, zox_children_capacity, zox_id(SwingState), 0);
        for (uint j = 0; j < length; j++) {
            SwingState* swing = zox_getm(bones[j], SwingState);
            if (swing->value) {
                // NOTE: 1 is idle and 2 is moving
                swing->value = state->value + 1;
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(arm_control_system);
