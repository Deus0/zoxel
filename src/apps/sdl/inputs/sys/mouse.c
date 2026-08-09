// NOTE: SDL doesn't do multiple mouses
// NOTE: This flips mouse position to match our engine

zox_sys2(MouseExtractSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_geter_value_non_const(main_app, WindowSize, int2, screen_size);
    if (screen_size.x % 2 != 0) {
        screen_size.x--;
    }
    if (screen_size.y % 2 != 0) {
        screen_size.y--;
    }
    int2 mouse_position;
    uint buttons = zox_sdl_get_mouse_state(&mouse_position);
    int2_flip_y(&mouse_position, screen_size);
    byte button_pressed_left = 0;
    byte button_pressed_right = 0;
    if (sdl_mouse_button_pressed(buttons, SDL_BUTTON_LEFT)) {
        button_pressed_left = 1;
    }
    if (sdl_mouse_button_pressed(buttons, SDL_BUTTON_RIGHT)) {
        button_pressed_right = 1;
    }
    zox_sys_begin();
    zox_sys_in(AppLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(AppLink, app);
        // using button_pressed_left
        uint children_capacity = zox_children_capacity;
        entity children[children_capacity];
        uint children_length = zox_get_children(world, e, children, children_capacity);
        for (uint j = 0; j < children_length; j++) {
            entity e2 = children[j];
            if (!zox_valid(e2)) {
                continue;
            }
            if (zox_has(e2, ZevicePointerPosition)) {
                if (global_any_fingers_down) {
                    zox_geter(e2, ZevicePointerPosition, position)
                    int2 position2 = position->value;
                    int2_flip_y(&position2, screen_size);
                    SDL_Window* sdl_window = zox_get_value(app->value, SDLWindow);
                    SDL_WarpMouseInWindow(sdl_window, position2.x, position2.y);
                } else {
                    zox_muter(e2, ZevicePointerPosition, position)
                    zox_muter(e2, ZevicePointerDelta, delta)
                    delta->value = int2_sub(mouse_position, position->value);
                    position->value = mouse_position;
                }
            }
            if (global_any_fingers_down) {
                continue;  // does this break it?
            }
            if (zox_has(e2, ZevicePointer)) {
                zox_muter(e2, ZevicePointer, clicker)
                clicker->value = get_button_click_state(clicker->value, button_pressed_left);
                if (dbg_log && clicker->value) {
                    zox_log("Mouse Left Clicked [%ix%i]", clicker->value);
                }
                // Mouse Pointer also has this
                if (zox_has(e2, ZevicePointerRight)) {
                    zox_muter(e2, ZevicePointerRight, clicker);
                    clicker->value = get_button_click_state(clicker->value, button_pressed_right);
                    if (dbg_log && clicker->value) {
                        zox_log("Mouse Right Clicked [%ix%i]", clicker->value);
                    }
                }
            } else if (zox_has(e2, ZeviceWheel)) {
                zox_muter(e2, ZeviceWheel, wheel);
                if (!int2_equals(static_mouse_wheel, wheel->value)) {
                    wheel->value = static_mouse_wheel;
                    if (dbg_log) {
                        zox_log("Mouse Wheel [%ix%i]", wheel->value.x, wheel->value.y);
                    }
                }
            }
        }
    }
} zox_sys_end(MouseExtractSystem);
