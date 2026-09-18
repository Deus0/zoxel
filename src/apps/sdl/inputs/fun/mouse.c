// #define zox_debug_log_extract_mouse

void sdl_extract_mouse_wheel(SDL_Event event) {
    byte dbg_log = 0;
    if (event.type == ZOX_MOUSEWHEEL) {
        static_mouse_wheel = (int2) {
            event.wheel.x,
            event.wheel.y
        };
        if (dbg_log &&
            (static_mouse_wheel.x ||
            static_mouse_wheel.y))
        {
            zox_log("Wheel Input [%ix%i]",
                static_mouse_wheel.x,
                static_mouse_wheel.y);
        }
    }
}

int2 get_mouse_center_point(
    ecs* world,
    entity e)
{
    zox_geter_value(e, WindowSize, int2, size);
    size = int2_half(size);
    if (zox_getv(e, WindowMaximized)) {
    //    size = get_window_size_without_header(world, e, size);
    }
    return size;
}
