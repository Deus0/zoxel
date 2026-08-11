// #define zox_debug_log_element_raycasting
int touch_devices_count = 0;
int touch_fingers_count = 0;

SDL_Finger* find_finger(int finger_id) {
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
            if (finger_id == (finger->id + 1)) {
                return finger;
            }
        }
    }
    return NULL;
}

void set_id(ecs *world, entity e, int new_id) {
    if (!zox_has(e, ID)) {
        zox_log_error("Invalid zevice, no ID");
        return;
    }
    zox_muter(e, ID, id);
    id->value = new_id;
}

void finger_released(ecs *world, entity e) {
    zox_muter(e, ZevicePointer, pointer);
    if (devices_get_pressed(pointer->value)) {
        devices_set_is_pressed(&pointer->value, 0);
        devices_set_released_this_frame(&pointer->value, 1);
        set_id(world, e, 0);
        global_any_fingers_down = 1; // one more frame
    }
}

byte sdl_extract_finger(ecs *world, int2 screen_size, entity e) {
    touch_fingers_count = 0;
    touch_devices_count = sdl_get_touch_device_count();
    if (!zox_valid(e) || !zox_has(e, Finger)) {
        return 0;
    }
    zox_geter_value(e, ID, int, zevice_id);
    if (!zevice_id) {
        return 1;   // to assign this finger
    }
    SDL_Finger* finger = find_finger(zevice_id);
    if (finger) {
        zox_muter(e, ZevicePointerPosition, zevicePointerPosition)
        zox_muter(e, ZevicePointerDelta, zevicePointerDelta)
        int2 finger_position = (int2) { (int) (finger->x * screen_size.x), (int) (finger->y * screen_size.y) };
        int2_flip_y(&finger_position, screen_size);
        if (!int2_equals(zevicePointerPosition->value, int2_hidden)) {
            int2 delta_finger = int2_sub_int2_(finger_position, zevicePointerPosition->value);
            zevicePointerDelta->value = delta_finger;
        }
        zevicePointerPosition->value = finger_position;
        global_any_fingers_down = 1;
        zox_logv(" > finger touching [%lu] fingerid [%i]", e, zevice_id);
    } else {
        finger_released(world, e);
        zox_logv(" - finger released [%lu] fingerid [%i]", e, zevice_id);
    }
    return 0;
}

byte touchscreen_has_id(ecs *world, int id, const entity* fingers, uint length) {
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

SDL_Finger* find_finger_unused(ecs *world, const entity* fingers, uint length) {
    for (int i = 0; i < touch_devices_count; i++) {
        SDL_TouchID touchscreen_id = sdl_get_touch_device(i);
        int fingers_count = sdl_get_touch_finger_count(touchscreen_id);
        touch_fingers_count += fingers_count;
        for (uint k = 0; k < length; k++) {
            SDL_Finger* finger = sdl_get_touch_finger(touchscreen_id, k);
            if (!finger) {
                // zox_logw("Finger null at [%i] of [%i]", k, fingers_count);
                continue;
            }
            int new_id = finger->id + 1;
            if (!touchscreen_has_id(world, new_id, fingers, length)) {
                return finger;
            }
        }
    }
    return NULL;
}

void sdl_assign_finger(ecs *world, int2 screen_size, const entity* children, uint children_length, entity e, byte dbg_log) {
    // get unused finger! find a finger that isn't used yet
    SDL_Finger* finger = find_finger_unused(world, children, children_length);
    if (!finger) {
        return;
    }
    int finger_id = finger->id + 1;
    set_id(world, e, finger_id);
    zox_muter(e, ZevicePointer, click);
    zox_muter(e, ZevicePointerPosition, position);
    click->value = 0;
    devices_set_pressed_this_frame(&click->value, 1);
    devices_set_is_pressed(&click->value, 1);
    int2 new_position = (int2) { (int) (finger->x * screen_size.x), (int) (finger->y * screen_size.y) };
    int2_flip_y(&new_position, screen_size);
    position->value = new_position;
    global_any_fingers_down = 1;
    if (dbg_log) {
        zox_log("New Finger [%s] fingerid [%i] at [%ix%i] Clicked [%i]", zox_getn(e), finger_id, new_position.x, new_position.y, click->value);
    }
}

zox_sys2(TouchscreenExtractSystem) {
    byte dbg_log = 0;
    global_any_fingers_down = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(AppLink);
    zox_sys_out(ScreenDimensions);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(AppLink, app);
        zox_sys_o(ScreenDimensions, screen_size);
        zox_geter_value(app->value, WindowSize, int2, window_size);
        screen_size->value = window_size;
        uint children_capacity = zox_children_capacity;
        entity children[children_capacity];
        uint children_length = zox_get_children(world, e, children, children_capacity);
        for (uint j = 0; j < children_length; j++) {
            entity e2 = children[j];
            if (sdl_extract_finger(world, window_size, e2)) {
                sdl_assign_finger(world, window_size, children, children_length, e2, dbg_log);
            }
        }
    }
} zox_sys_end(TouchscreenExtractSystem);
