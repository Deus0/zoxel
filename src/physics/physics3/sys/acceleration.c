// linear drag of Frictioned bodies, velocity decays as e^(-k t), see physics/fun
float ground_friction3 = 6;
float air_friction3 = 1;

// moves bodies with an Acceleration3D: exact steps (see physics/fun), then clears the forces
//  - by each body's clock (physics_body_dt): paused, its realm's is 0, it stays
static inline void physics3_accelerate(iter* it, const float k, const float dt) {
    const physics_step frame_step = physics_step_new(k, dt);
    zox_sys_begin();
    zox_sys_out(Acceleration3D);
    zox_sys_out(Velocity3D);
    zox_sys_out(Position3D);
    zox_sys_in(ClockDelta);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Acceleration3D, acceleration);
        zox_sys_o(Velocity3D, velocity);
        zox_sys_o(Position3D, position);
        const physics_step step = ClockDelta_ ? physics_step_new(k, ClockDelta_[i].value) : frame_step;
        position->value = float3_add(position->value, physics_step_apply3(step, &velocity->value, acceleration->value));
        float3_make_zero(&acceleration->value);
    }
}

// not Frictioned, or Frictioned without a Grounded: no drag
void acceleration3_system(iter* it) {
    if (zox_delta_time > zox_physics_max_delta_time) {
        return;
    }
    zox_sys_on_begin();
    zox_physics_profile_begin();
    physics3_accelerate(it, 0, (float) zox_delta_time);
    zox_physics_profile_end();
    zox_sys_on_end();
} zoxd_system(acceleration3_system);

void acceleration3_loose_system(iter* it) {
    zox_sys_on_begin();
    acceleration3_system(it);
    zox_sys_on_end();
} zoxd_system(acceleration3_loose_system);

// Frictioned and Grounded: drag, stronger on the ground
void friction3_system(iter* it) {
    if (zox_delta_time > zox_physics_max_delta_time) {
        return;
    }
    zox_sys_on_begin();
    zox_physics_profile_begin();
    const float dt = (float) zox_delta_time;
    const physics_step ground = physics_step_new(ground_friction3, dt);
    const physics_step air = physics_step_new(air_friction3, dt);
    zox_sys_begin();
    zox_sys_in(Grounded);
    zox_sys_out(Acceleration3D);
    zox_sys_out(Velocity3D);
    zox_sys_out(Position3D);
    zox_sys_in(ClockDelta);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Grounded, grounded);
        zox_sys_o(Acceleration3D, acceleration);
        zox_sys_o(Velocity3D, velocity);
        zox_sys_o(Position3D, position);
        const physics_step step = ClockDelta_ ?
            physics_step_new(grounded->value ? ground_friction3 : air_friction3, ClockDelta_[i].value) :
            (grounded->value ? ground : air);
        position->value = float3_add(position->value, physics_step_apply3(step, &velocity->value, acceleration->value));
        float3_make_zero(&acceleration->value);
    }
    zox_physics_profile_end();
    zox_sys_on_end();
} zoxd_system(friction3_system);

// the scale a velocity needs to move v * dt this frame under the drag it gets (friction3_system: ground drag when
//  Grounded, air drag when not; no drag without Frictioned and Grounded), for movement that sets a velocity (XR
//  walking): drag that changes as Grounded flickers (block seams) no longer changes how far it moves, no bumps
//  - read before zoxp_physics, the same Grounded and zox_delta_time the step uses this frame
static inline float physics3_drag_compensation(ecs* world, entity e) {
    const float dt = (float) zox_delta_time;
    if (!(dt > 0) || !zox_has(e, Frictioned) || !zox_has(e, Grounded) || !zox_has(e, Acceleration3D)) {
        return 1;
    }
    const physics_step step = physics_step_new(zox_getv(e, Grounded) ? ground_friction3 : air_friction3, dt);
    return step.g > 0 ? dt / step.g : 1;
}
