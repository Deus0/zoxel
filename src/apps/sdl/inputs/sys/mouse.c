// NOTE: SDL doesn't do multiple mouses
// NOTE: This flips mouse position to match our engine

// is the mouse in the window (else it's nowhere: what it hovered unhovers, its tooltip goes)
//  - alt tabbing back in (the pointer already over it, always when fullscreen), sdl may not give it mouse focus till
//    the pointer moves (wayland), so it's in while the window has keyboard focus and the pointer's inside it
byte sdl_mouse_in_window(byte mouse_focus, byte keyboard_focus, int2 position, int2 size) {
    const byte in_bounds = position.x >= 0 && position.y >= 0 && position.x < size.x && position.y < size.y;
    return mouse_focus || (keyboard_focus && in_bounds);
}

void mouse_input_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    int2 global_mouse_position;
    uint buttons = zox_sdl_get_mouse_state(&global_mouse_position);
    byte button_pressed_left = 0;
    byte button_pressed_right = 0;
    if (sdl_mouse_button_pressed(buttons, SDL_BUTTON_LEFT)) {
        button_pressed_left = 1;
    }
    if (sdl_mouse_button_pressed(buttons, SDL_BUTTON_RIGHT)) {
        button_pressed_right = 1;
    }
    if (zox_mouse_fake) {
        button_pressed_left = zox_mouse_fake_left;
        button_pressed_right = zox_mouse_fake_right;
    }
    // zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity app = zox_get_link(world, e, AppLink);
        if (!zox_valid(app) ||
            !zox_has(app, SDLWindow) ||
            !zox_has(app, WindowSize))
        {
            continue;
        }
        int2 screen_size = zox_getv(app, WindowSize);
        if (screen_size.x % 2 != 0) {
            screen_size.x--;
        }
        if (screen_size.y % 2 != 0) {
            screen_size.y--;
        }
        int2 mouse_position = global_mouse_position;
        int2_flip_y(&mouse_position, screen_size);
        // out of the window it's nowhere (sdl keeps its last place inside: what it hovered stayed hovered, its
        //  tooltip stuck)
        SDL_Window* window = zox_getv(app, SDLWindow);
        const byte in_window = sdl_mouse_in_window(SDL_GetMouseFocus() == window, SDL_GetKeyboardFocus() == window,
            global_mouse_position, screen_size);
        if (!in_window) {
            mouse_position = int2_hidden;
        }
        if (zox_mouse_fake) {
            mouse_position = zox_mouse_fake_position;
        }
        // using button_pressed_left
        uint children_capacity = zox_children_capacity;
        entity children[children_capacity];
        uint children_length = zox_get_children(
            world,
            e,
            children,
            children_capacity);
        for (uint j = 0; j < children_length; j++) {
            entity e2 = children[j];
            if (!zox_valid(e2)) {
                continue;
            }
            if (zox_has(e2, ZevicePointerPosition)) {
                if (global_any_fingers_down) {
                    const ZevicePointerPosition* position = zox_get(e2, ZevicePointerPosition);
                    int2 position2 = position->value;
                    int2_flip_y(&position2, screen_size);
                    SDL_Window* sdl_window = zox_getv(app, SDLWindow);
                    SDL_WarpMouseInWindow(sdl_window, position2.x, position2.y);
                } else {
                    zox_muter(e2, ZevicePointerPosition, position);
                    zox_muter(e2, ZevicePointerDelta, delta);
                    // no jump leaving or coming back into the window
                    byte hidden = int2_equals(mouse_position, int2_hidden) || int2_equals(position->value, int2_hidden);
#ifdef zox_web
                    // locked but the browser's lock is lost (alt tab): sdl gives where the cursor is, not how far
                    //  it moved, and the warp to the centre does nothing there, so every frame read the cursor's
                    //  distance from the centre as movement and the camera spun; still till a click locks it again
                    if (zox_has(e, MouseLock) && zox_getv(e, MouseLock) && !web_pointer_locked()) {
                        hidden = 1;
                    }
#endif
                    delta->value = hidden ? int2_zero : int2_sub(mouse_position, position->value);
                    position->value = mouse_position;
                }
            }
            if (global_any_fingers_down) {
                continue;  // does this break it?
            }
            if (zox_has(e2, ZevicePointer)) {
                zox_muter(e2, ZevicePointer, clicker);
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
            }
            // can be both
            if (zox_has(e2, ZeviceWheel)) {
                zox_muter(e2, ZeviceWheel, wheel);
                const int2 mouse_wheel = zox_mouse_fake ? zox_mouse_fake_wheel : static_mouse_wheel;
                if (!int2_equals(
                    mouse_wheel,
                    wheel->value))
                {
                    wheel->value = mouse_wheel;
                    if (dbg_log) {
                        zox_log("Mouse Wheel [%ix%i]",
                            wheel->value.x,
                            wheel->value.y);
                    }
                }
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(mouse_input_system);
