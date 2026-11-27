// The move (doc/plans/movement.md): every player character walks by its MoveDirection and turns to its FaceDirection,
//  written by its player's input (movement.c), later its ai: by the same rules whoever wants it
//  - turning: by physics (a torque on its spin, as npcs turn), or at once (player_turn_instant, the old turn)
//  - the push: along its move in its own frame, each axis its power (player_movement_power), held back while that
//    axis would pass its top speed (max_velocity3D); running, crouching, water and flying scale them; the speed stat
//    raises both (player_speed_stat_scale)
//  - each axis's push trimmed to what lands it on its top speed, its drag counted (exact at any frame rate), not dropped
//    whole: dropped, at ~0.07s frames a push
//    was most of the top speed, so it was held back till friction stopped it, strafing in pulses (tst move_feel_10fps)
//  - kept as it was, for the same feel (tst move_feel): running scales only the side push (movement.x and .y):
//    forward runs on its top speed alone

// the rotation that faces a direction (flat): characters face +z (is_camera_positive_z)
static inline float4 character3_facing(float3 direction) {
    const float yaw = is_camera_positive_z ? atan2f(direction.x, direction.z) : atan2f(-direction.x, -direction.z);
    return quaternion_from_axis_angle(float3_up, yaw);
}

// turns it to face a direction: at once, or a torque on its spin (a spring to it, damped by its spin)
static inline void character3_turn(float4* rotation, Alpha3D* alpha, float omega_y, float3 face) {
    if (player_turn_instant) {
        *rotation = character3_facing(face);
        return;
    }
    const float wanted = is_camera_positive_z ? atan2f(face.x, face.z) : atan2f(-face.x, -face.z);
    const float error = remainderf(wanted - quaternion_to_euler_y2(*rotation), 2 * M_PI);
    if (fabsf(error) < 0.001f && fabsf(omega_y) < 0.01f) {
        return;
    }
    const float torque = player_turn_strength * error - player_turn_damping * omega_y;
    alpha->value.y += clampf(torque, -player_turn_max, player_turn_max);
}

// an axis's push this frame, trimmed so its velocity reaches its top speed and no further, with the drag it has
//  this frame (physics/fun: v' = v keep + a g, exact at any frame rate): velocity, the push already on it this frame,
//  the push wanted (all along the axis), its top speed
static inline float character3_axis_push(physics_step step, float velocity, float pushed, float push, float top) {
    if (!push || step.g <= 0) {
        return push;
    }
    const float limit = push > 0 ? top : -top;
    const float room = (limit - velocity * step.keep) / step.g - pushed;   // the push that lands on the top speed
    if (room * push <= 0) {
        return 0;       // at or past it already
    }
    return fabsf(room) < fabsf(push) ? room : push;
}

// its drag this frame, as physics3 steps it (its friction on the ground or in the air, none unfrictioned)
static inline physics_step character3_step(ecs* world, entity e) {
    const float k = !zox_has(e, Frictioned) ? 0 : zox_has(e, Grounded) && zox_getv(e, Grounded) ? ground_friction3 : air_friction3;
    return physics_step_new(k, (float) zox_delta_time);
}


void character3_move_system(iter* it) {
    zox_sys_on_begin();
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MoveDirection);
    zox_sys_in(FaceDirection);
    zox_sys_in(Velocity3D);
    zox_sys_in(Omega3D);
    zox_sys_out(Rotation3D);
    zox_sys_out(Acceleration3D);
    zox_sys_out(Alpha3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MoveDirection, move);
        zox_sys_i(FaceDirection, face);
        zox_sys_i(Velocity3D, velocity);
        zox_sys_i(Omega3D, omega);
        zox_sys_o(Rotation3D, rotation);
        zox_sys_o(Acceleration3D, acceleration);
        zox_sys_o(Alpha3D, alpha);
        if (zox_has(e, DisableMovement)) {
            continue;
        }
        // its speed limits measured against its facing before it turns this frame (as the old move)
        const float4 facing = rotation->value;
        if (face->value.x || face->value.z) {
            character3_turn(&rotation->value, alpha, omega->value.y, face->value);
        }
        if (!move->value.x && !move->value.z && !move->value.y) {
            continue;
        }
        const byte flying = zox_has(e, FlyMode) && zox_getv(e, FlyMode);
        const byte crouching = character3_crouching(world, e);
        const byte running = zox_has(e, Running) && !crouching;
        const float speed = character3_speed_scale(world, e);
        // its move in its own frame (as it now faces), each axis its power
        const float4 frame = rotation->value;
        const float3 local = float4_rotate_float3(float4_inverse(frame), move->value);
        float3 movement = { local.x * player_movement_power.x * speed, 0, local.z * player_movement_power.y * speed };
        if (running) {
            const float acceleration_scale = flying ? fly_run_acc_xz : run_accceleration;
            movement.x *= acceleration_scale;
            movement.y *= acceleration_scale;
        }
        // an axis asked past 1 (a headset's body catching its view up, movement.c player3_move_xr) may go that much
        //  faster, to half again: players' sticks never ask past 1
        const float2 top = character3_top_speed(world, e);
        const float2 max_speed = { top.x * clampf(fabsf(local.x), 1, 1.5f), top.y * clampf(fabsf(local.z), 1, 1.5f) };
        if (crouching) {
            movement.x *= crouch_speed;
            movement.z *= crouch_speed;
        }
        // along the ground: flat against its up (its gravity's, any way)
        const float3 up = character_up(world, e);
        const float3 push_z = physics3_flatten(float4_rotate_float3(frame, (float3) { 0, 0, movement.z }), up);
        const float3 push_x = physics3_flatten(float4_rotate_float3(frame, (float3) { movement.x, 0, 0 }), up);
        // held back while the axis would pass its top speed (its velocity and push measured as it faced)
        const float4 inverse = float4_inverse(facing);
        const float3 velocity_local = float4_rotate_float3(inverse, velocity->value);
        const float3 acceleration_local = float4_rotate_float3(inverse, acceleration->value);
        const physics_step step = character3_step(world, e);
        const float push_forward = character3_axis_push(step, velocity_local.z, acceleration_local.z, movement.z, max_speed.y);
        const float push_side = character3_axis_push(step, velocity_local.x, acceleration_local.x, movement.x, max_speed.x);
        if (movement.z) {
            acceleration->value = float3_add(acceleration->value, float3_scale(push_z, push_forward / movement.z));
        }
        if (movement.x) {
            acceleration->value = float3_add(acceleration->value, float3_scale(push_x, push_side / movement.x));
        }
    }
    zox_sys_on_end();
} zoxd_system(character3_move_system);
