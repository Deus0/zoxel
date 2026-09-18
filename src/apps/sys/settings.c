extern entity main_app;


void zox_toggle_fullscreen(ecs *world) {
    fullscreen = !fullscreen;
    if (zox_valid(main_app)) {
        zox_set_app_fullscreen(world, main_app, fullscreen);
    }
}

void zox_toggle_maximized(ecs *world) {
    maximized = !maximized;
    zox_log("TODO: Set Maximized Setting here.");
}
