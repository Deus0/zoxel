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

// a body's step this frame, as physics3 steps it: its drag on the ground or in the air, none unfrictioned
static inline physics_step physics3_body_step(ecs* world, entity e) {
    const float k = !zox_has(e, Frictioned) ? 0 : zox_has(e, Grounded) && zox_getv(e, Grounded) ? ground_friction3 : air_friction3;
    return physics_step_new(k, (float) zox_delta_time);
}

// a kick (a hit, a leap): one frame's push that changes its velocity by dv (v' = v keep + a g), added to its forces
//  (doc/tech/physics.md: forces, never its velocity); before zoxp_physics it lands this frame, else the next
static inline void physics3_kick(ecs* world, entity e, float3 dv) {
    if (!zox_has(e, Acceleration3D)) {
        return;
    }
    const physics_step step = physics3_body_step(world, e);
    if (step.g <= 0) {
        return;
    }
    Acceleration3D* acceleration = zox_getm(e, Acceleration3D);
    acceleration->value = float3_add(acceleration->value, float3_scale(dv, 1 / step.g));
}
