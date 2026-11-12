void header_spawn_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();



    zox_sys_on_end();
} zoxd_system(header_spawn_system);
