// #define zox_debug_log_extract_mouse

void sdl_extract_mouse_wheel(SDL_Event event) {
    if (event.type == ZOX_MOUSEWHEEL) {
        static_mouse_wheel = (int2) { event.wheel.x, event.wheel.y };
    }
}

int2 get_mouse_center_point(ecs *world, entity e) {
    zox_geter_value(e, WindowSize, int2, size);
    size = int2_half(size);
    if (zox_getv(e, WindowMaximized)) {
    //    size = get_window_size_without_header(world, e, size);
    }
    return size;
}
