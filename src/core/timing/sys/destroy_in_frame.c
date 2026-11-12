void destroy_in_frame_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_delete(e);
    }
    zox_sys_on_end();
} zoxd_system(destroy_in_frame_system);
