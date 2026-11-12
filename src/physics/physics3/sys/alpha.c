// Spin: Omega3D (angular velocity) and Alpha3D (angular acceleration), in the body's frame
//  - exact steps like linear motion (see physics/fun), Frictioned bodies also slow their spin
//      with omega_dissipation3, so a spin turns the same at any frame rate
//  - bodies with an Alpha3D spin in spin3_system / spin3_damped_system (and it's cleared),
//      the rest in Omega3System / Omega3DampedSystem
//  - the turn is about the step's average axis, exact when alpha is along omega (or zero)

// turns a rotation by a body frame rotation vector (axis * angle)
static inline void physics3_turn(float4* rotation, const float3 turn, const float dt) {
    const float angle = float3_length(turn);
    if (angle < 1e-6f * dt) {
        return; // no rotation this frame
    }
    quaternion_rotate_quaternion_p(rotation, quaternion_from_axis_angle(float3_scale(turn, 1.0f / angle), angle));
    const float length = sqrtf(rotation->x * rotation->x + rotation->y * rotation->y + rotation->z * rotation->z + rotation->w * rotation->w);
    float4_multiply_float_p(rotation, 1.0f / length);
}

static inline void physics3_spin(iter* it, const float c) {
    const float dt = (float) zox_delta_time;
    const physics_step step = physics_step_new(c, dt);
    zox_physics_profile_begin();
    zox_sys_begin();
    zox_sys_out(Alpha3D);
    zox_sys_out(Omega3D);
    zox_sys_out(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Alpha3D, alpha);
        zox_sys_o(Omega3D, omega);
        zox_sys_o(Rotation3D, rotation);
        const float3 turn = physics_step_apply3(step, &omega->value, alpha->value);
        physics3_turn(&rotation->value, turn, dt);
        float3_make_zero(&alpha->value);
    }
    zox_physics_profile_end();
}

static inline void physics3_spin_free(iter* it, const float c) {
    const float dt = (float) zox_delta_time;
    const physics_step step = physics_step_new(c, dt);
    zox_physics_profile_begin();
    zox_sys_begin();
    zox_sys_out(Omega3D);
    zox_sys_out(Rotation3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Omega3D, omega);
        zox_sys_o(Rotation3D, rotation);
        const float3 turn = physics_step_apply3(step, &omega->value, float3_zero);
        physics3_turn(&rotation->value, turn, dt);
    }
    zox_physics_profile_end();
}

void spin3_system(iter* it) {
    zox_sys_on_begin();
    if (zox_delta_time > zox_physics_max_delta_time) {
        return;
    }
    physics3_spin(it, 0);
    zox_sys_on_end();
} zoxd_system(spin3_system);

void spin3_damped_system(iter* it) {
    zox_sys_on_begin();
    if (zox_delta_time > zox_physics_max_delta_time) {
        return;
    }
    physics3_spin(it, omega_dissipation3);
    zox_sys_on_end();
} zoxd_system(spin3_damped_system);

void Omega3System(iter* it) {
    zox_sys_on_begin();
    if (zox_delta_time > zox_physics_max_delta_time) {
        return;
    }
    physics3_spin_free(it, 0);
    zox_sys_on_end();
} zoxd_system(Omega3System);

void Omega3DampedSystem(iter* it) {
    zox_sys_on_begin();
    if (zox_delta_time > zox_physics_max_delta_time) {
        return;
    }
    physics3_spin_free(it, omega_dissipation3);
    zox_sys_on_end();
} zoxd_system(Omega3DampedSystem);
