// #define zox_debug_log_element_raycasting
// TODO: Make a FingerExtract system, so we can run from a finger level
int touch_devices_count = 0;
int touch_fingers_count = 0;

SDL_Finger* find_finger(int id) {
    for (int j = 0; j < touch_devices_count; j++) {
        SDL_TouchID touchscreen_id = sdl_get_touch_device(j);
        int fingers_count = sdl_get_touch_finger_count(touchscreen_id);
        touch_fingers_count += fingers_count;
        for (int k = 0; k < fingers_count; k++) {
            SDL_Finger *finger = sdl_get_touch_finger(touchscreen_id, k);
            if (!finger) {
                // zox_logw("Finger null at [%i] of [%i]", k, fingers_count);
                continue;
            }
            if (id == (finger->id + 1)) {
                return finger;
            }
        }
    }
    return NULL;
}

byte sdl_extract_finger(ecs *world, int2 screen_size, entity e, byte dbg_log) {
    // If release last frame, finally reset it
    byte clicked = zox_getv(e, ZevicePointer);
    // NOTE: If released last frame, reset all the values now
    if (devices_get_released_this_frame(clicked)) {
        zox_setm(e, ZevicePointer, 0);
        zox_setm(e, ZeviceDisabled, 1);
        zox_setm(e, ZevicePointerPosition, int2_hidden);
        zox_setm(e, ZevicePointerDelta, int2_zero);
        zox_setm(e, ID, 0);
        if (dbg_log) {
            zox_log("Finger Released Post Frame [%s]", zox_getn(e));
        }
        return 0;
    } else if (devices_get_pressed_last_frame(clicked)) {
        zox_muter(e, ZevicePointer, clickedm);
        devices_set_pressed_last_frame(&clickedm->value, 0);
        if (dbg_log) {
            zox_log("Finger Pressed Last, Last Frame [%s]", zox_getn(e));
        }
    } else if (devices_get_pressed_this_frame(clicked)) {
        zox_muter(e, ZevicePointer, clickedm);
        devices_set_pressed_this_frame(&clickedm->value, 0);
        devices_set_pressed_last_frame(&clickedm->value, 1);
        if (dbg_log) {
            zox_log("Finger Pressed Last Frame [%s] - Value [%i]", zox_getn(e), clickedm->value);
        }
    }
    touch_fingers_count = 0;
    int zevice_id = zox_getv(e, ID);
    SDL_Finger* finger = find_finger(zevice_id);
    if (!finger) {
        zox_muter(e, ZevicePointer, clickedm);
        clickedm->value = 0;
        devices_set_released_this_frame(&clickedm->value, 1);
        if (dbg_log) {
            zox_log("Finger Released [%s] ID [%i]", zox_getn(e), zevice_id);
        }
        return 1;
    }
    zox_muter(e, ZevicePointerPosition, position);
    zox_muter(e, ZevicePointerDelta, delta);
    int2 new_position = (int2) { (int) (finger->x * screen_size.x), (int) (finger->y * screen_size.y) };
    int2_flip_y(&new_position, screen_size);
    if (!int2_equals(position->value, int2_hidden)) {
        delta->value = int2_sub_int2_(new_position, position->value);
    }
    position->value = new_position;
    if (dbg_log) {
        zox_log("   - Finger Touching [%s] ID [%i]", zox_getn(e), zevice_id);
    }
    return 1;
}

byte touchscreen_has_id(ecs *world, const entity* fingers, uint length, int id) {
    for (uint i = 0; i < length; i++) {
        entity finger = fingers[i];
        if (!zox_has(finger, Finger)) {
            continue;
        }
        if (id == zox_getv(finger, ID)) {
            return 1;
        }
    }
    return 0;
}

// NOTE: Uses fingers (device children) to check if id exists
SDL_Finger* find_new_finger(ecs *world, const entity* fingers, uint length, byte dbg_log) {
    if (dbg_log >= 3) {
        zox_log("Touchscreens [%i]", touch_devices_count);
    }
    for (int i = 0; i < touch_devices_count; i++) {
        SDL_TouchID touchscreen_id = sdl_get_touch_device(i);
        int fingers_count = sdl_get_touch_finger_count(touchscreen_id);
        touch_fingers_count += fingers_count;
        if (dbg_log >= 2) {
            zox_log("Touchscreen has fingers down [%i]", fingers_count);
        }
        for (uint k = 0; k < fingers_count; k++) {
            SDL_Finger* finger = sdl_get_touch_finger(touchscreen_id, k);
            if (!finger) {
                zox_loge("Finger null at [%i] of [%i]", k, fingers_count);
                continue;
            }
            int new_id = finger->id + 1;
            if (!touchscreen_has_id(world, fingers, length, new_id)) {
                return finger;
            } else {
                if (dbg_log >= 2) {
                    zox_log("Touchscreen has finger [%i]", new_id);
                }
            }
        }
    }
    return NULL;
}

byte link_sdl_new_finger(ecs *world, int2 screen_size, const entity* children, uint children_length, entity e, byte dbg_log) {
    // get unused finger! find a finger that isn't used yet
    SDL_Finger* finger = find_new_finger(world, children, children_length, dbg_log);
    if (!finger) {
        if (dbg_log >= 3) {
            zox_log("No new finger this frame");
        }
        return 0;
    }
    int finger_id = finger->id + 1;
    int2 position = (int2) { (int) (finger->x * screen_size.x), (int) (finger->y * screen_size.y) };
    int2_flip_y(&position, screen_size);
    byte clicked = 0;
    devices_set_pressed_this_frame(&clicked, 1);
    devices_set_is_pressed(&clicked, 1);
    zox_setm(e, ZevicePointer, clicked);
    zox_setm(e, ZevicePointerPosition, position);
    zox_setm(e, ID, finger_id);
    zox_setm(e, ZeviceDisabled, 0);
    if (dbg_log) {
        zox_log("New Finger [%s] ID [%i] at [%ix%i] Clicked [%i]", zox_getn(e), finger_id, position.x, position.y, clicked);
    }
    return 1;
}

zox_sys2(TouchscreenExtractSystem) {
    byte dbg_log = 0;
    // TODO: Remove this, its bad
    global_any_fingers_down = 0;
    touch_devices_count = sdl_get_touch_device_count();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(PixelSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(PixelSize, screen_size);
        entity app = zox_get_link(world, e, App);
        if (!zox_valid(app) || !zox_has(app, WindowSize)) {
            continue;
        }
        screen_size->value = zox_getv(app, WindowSize);
        // For all Zevices
        uint children_capacity = zox_children_capacity;
        entity children[children_capacity];
        uint children_length = zox_get_children(
            world,
            e,
            children,
            children_capacity);
        for (uint j = 0; j < children_length; j++) {
            entity e2 = children[j];
            // NOTE: Virtual joystick zevice has no finger
            if (!zox_has(e2, Finger)) {
                continue;
            }
#ifdef zox_safety_checks
            if (!zox_has(e2, ID)) {
                zox_loge("Zevice has wrong components [%s]", zox_getn(e2));
                continue;
            }
#endif
            // When disabled check for new finger
            if (zox_getv(e2, ZeviceDisabled)) {
                if (link_sdl_new_finger(
                    world,
                    screen_size->value,
                    children,
                    children_length,
                    e2,
                    dbg_log))
                {
                    global_any_fingers_down = 1;
                }
            } else {
                if (sdl_extract_finger(
                    world,
                    screen_size->value,
                    e2,
                    dbg_log))
                {
                    global_any_fingers_down = 1;
                }
            }
        }
    }
} zox_sys_end(TouchscreenExtractSystem);
