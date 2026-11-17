// the 3D "Loading" label eases to ahead of its player's camera (huds3_follow), so it stays in view as the head turns
void huds3_follow_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        huds3_follow(world, e, zox_delta_time);
    }
    zox_sys_on_end();
} zoxd_system(huds3_follow_system);
