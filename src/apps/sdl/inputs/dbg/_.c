uint get_debug_label_app(ecs *world, entity player, char *buffer, uint size, uint index) {

    (void) player;
    zox_geter_value(main_app, WindowSize, int2, window_size)

    int2 lock_position = get_mouse_center_point(world, main_app);

    zox_geter_value(main_app, WindowFullscreen, byte, fullscreen)
    zox_geter_value(main_app, WindowMaximized, byte, maximized)
    index += snprintf(buffer + index, size - index, " - window_size %ix%i\n", window_size.x, window_size.y);
    index += snprintf(buffer + index, size - index, " - lock_position %ix%i\n", lock_position.x, lock_position.y);
    index += snprintf(buffer + index, size - index, " - fullscreen %i\n", fullscreen);
    index += snprintf(buffer + index, size - index, " - maximized %i\n", maximized);
    return index;
}
