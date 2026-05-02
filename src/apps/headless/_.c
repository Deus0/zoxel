static inline int2 get_screen_size() {
    return int2_zero;
}

void on_set_vsync(byte value) { }

void zox_set_app_fullscreen(ecs* world, entity e, byte fullscreen) { }
void zox_app_set_monitor_e(ecs *world, entity e, byte monitor) { }
void zox_set_app_maximized(ecs* world, entity e, byte maximized) { }


byte apps_is_extension_supported(const char* name) {
    return 0;
}
