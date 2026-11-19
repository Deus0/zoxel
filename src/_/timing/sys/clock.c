// Clocks: each its own time, ECS all the way down (doc/plans/realm_clock.md)
//  - a realm's clock follows the frame's (zox_delta_time), its terrain's its realm's, a thing's (a character, a
//    projectile) its terrain's: ClockLink; ClockScale its rate of the one it follows, a realm's 0 while paused
//  - its ClockDelta is what its systems move by: the delta of the clock it follows times its scale, from the top of
//    its chain each frame (a chain of three), so no clock reads another's from the frame before
#define clock_chain_max 8

// the seconds a clock passes this frame: its scale, by the scales up its links, by the frame's
static inline float clock_delta_of(ecs* world, entity e) {
    float delta = (float) zox_delta_time;
    for (byte i = 0; i < clock_chain_max && zox_valid(e); i++) {
        if (zox_has(e, ClockScale)) {
            delta *= zox_getv(e, ClockScale);
        }
        e = zox_get_link(world, e, ClockLink);
    }
    return delta;
}

void clock_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(ClockTime);
    zox_sys_out(ClockDelta);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(ClockTime, time);
        zox_sys_o(ClockDelta, delta);
        delta->value = clock_delta_of(world, e);
        time->value += delta->value;
    }
    zox_sys_on_end();
} zoxd_system(clock_system);
