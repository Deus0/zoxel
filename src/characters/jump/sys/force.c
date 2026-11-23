float zox_jump_power = 6.0f;

// a jump pushes against its gravity (physics3_up, any direction: a gravity cube's flip), +y without one
void jump_force_system(iter* it) {
    zox_sys_on_begin();
    init_delta_time();
    zox_sys_begin();
    //zox_sys_in(Velocity3D);
    zox_sys_out(Acceleration3D);
    zox_sys_out(Jump)
    zox_sys_in(Gravity3D);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_i(Velocity3D, vel);
        zox_sys_o(Acceleration3D, acc);
        zox_sys_o(Jump, jump);
        if (!jump->value) {
            continue;
        }
        float dt = delta_time;
        if (dt > jump->value) {
            dt = jump->value;
        }
        const float3 up = physics3_up(Gravity3D_ ? Gravity3D_[i].value : float3_zero);
        acc->value = float3_add(acc->value, float3_scale(up, (zox_jump_power / zox_jump_time) * (dt / delta_time)));
        jump->value -= dt;
        //acc->value.y += zox_jump_power / zox_jump_time;
        // jump->value -= delta_time;
        if (jump->value < 0) {
            jump->value = 0;
        }
    }
    zox_sys_on_end();
} zoxd_system(jump_force_system);

/*void jump3_vel_system(iter* it) {
    zox_sys_on_begin();
    const float jump_power = zox_jump_power;
    init_delta_time();
    zox_sys_begin();
    zox_sys_out(Velocity3D);
    zox_sys_out(Jump);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Velocity3D, velocity3);
        zox_sys_o(Jump, jump);
        if (!jump->value) {
            continue;
        }
        if (jump->value > 0) {
            velocity3->value.y = jump_power; // Jump impulse
        }
        jump->value -= delta_time;
        if (jump->value < 0) {
            jump->value = 0;
        }
    }
    zox_sys_on_end();
} zoxd_system(jump3_vel_system);*/

/*float intended_vel = zox_jump_power;
float acc_total = (intended_vel - vel->value.y) / delta_time;
float acc_to_add = acc_total - acc->value.y;
acc->value.y += acc_to_add;
jump->value -= delta_time;
if (jump->value < 0) {
    jump->value = 0;
}*/