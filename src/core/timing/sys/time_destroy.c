// extern void zox_debug_sound(ecs* world, entity e);

void destroy_in_time_system(iter* it) {
    zox_sys_on_begin();
    init_delta_time();
    const float max_delta = 0.1f;  // 100 ms max step, tweak as needed
    float dt = delta_time > max_delta ? max_delta : delta_time;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(DestroyInTime);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(DestroyInTime, life);
        if (!life->value) {
            continue;
        }
        life->value -= dt;
        if (life->value <= 0) {
            // zox_debug_sound(world, e);
            zox_delete(e);
        }
    }
    zox_sys_on_end();
} zoxd_system(destroy_in_time_system);