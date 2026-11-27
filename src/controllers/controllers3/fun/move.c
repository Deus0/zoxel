// what the move (sys/move.c) and a headset's view (xr/fun/body.c) share: a character's up, how fast it may go

// a character's up, against its gravity (any direction: a gravity cube's flip), +y without one
static inline float3 character_up(ecs* world, entity character) {
    return physics3_up(zox_has(character, Gravity3D) ? zox_getv(character, Gravity3D) : float3_zero);
}

// how much the speed stat (1 a player's start) raises its top speed and push
static inline float character3_speed_scale(ecs* world, entity e) {
    return 1 + (character_speed(world, e) - 1) * player_speed_stat_scale;
}

// crouching: the skeleton's CrouchState (a headset crouches by the player's own body, not this)
static inline byte character3_crouching(ecs* world, entity e) {
    const entity skeleton = get_skeleton(world, e);
    return !zox_xr_active && zox_valid(skeleton) && zox_has(skeleton, CrouchState) && zox_getv(skeleton, CrouchState);
}

// its top speeds now, side and forward: the walk's (max_velocity3D) by the speed stat, crouching, running (flying's
//  own), in water
static inline float2 character3_top_speed(ecs* world, entity e) {
    const byte crouching = character3_crouching(world, e);
    const byte running = zox_has(e, Running) && !crouching;
    const byte flying = zox_has(e, FlyMode) && zox_getv(e, FlyMode);
    float2 top = float2_multiply1(max_velocity3D, character3_speed_scale(world, e));
    if (crouching) {
        top = float2_multiply1(top, crouch_speed);
    }
    if (running) {
        top = float2_multiply1(top, flying ? fly_run_speed : run_speed);
    }
    if (zox_has(e, InWater)) {
        top = float2_multiply1(top, swim_speed);
    }
    return top;
}
