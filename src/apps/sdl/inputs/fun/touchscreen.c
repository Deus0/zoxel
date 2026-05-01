// #define zox_debug_log_element_raycasting
int touch_devices_count = 0;
int touch_fingers_count = 0;

byte touchscreen_has_id(ecs *world, const Children *zevices, const int id) {
    for (int i = 0; i < zevices->length; i++) {
        const entity zevice = zevices->value[i];
        if (!zox_has(zevice, Finger)) {
            continue;
        }
        if (id == zox_gett_value(zevice, ID)) {
            return 1;
        }
    }
    return 0;
}

SDL_Finger* find_finger_unused(
    ecs *world,
    const Children *zevices
) {
    for (int i = 0; i < touch_devices_count; i++) {
        SDL_TouchID touchscreen_id = SDL_GetTouchDevice(i);
        int fingers_count = SDL_GetNumTouchFingers(touchscreen_id);
        touch_fingers_count += fingers_count;
        for (int k = 0; k < fingers_count; k++) {
            SDL_Finger *finger = SDL_GetTouchFinger(touchscreen_id, k);
            int new_id = finger->id + 1;
            if (!touchscreen_has_id(world, zevices, new_id)) {
                return finger;
            }
        }
    }
    return NULL;
}

SDL_Finger* find_finger(int finger_id) {
    for (int j = 0; j < touch_devices_count; j++) {
        SDL_TouchID touchscreen_id = SDL_GetTouchDevice(j);
        int fingers_count = SDL_GetNumTouchFingers(touchscreen_id);
        touch_fingers_count += fingers_count;
        for (int k = 0; k < fingers_count; k++) {
            SDL_Finger *finger = SDL_GetTouchFinger(touchscreen_id, k);
            if (finger_id == (finger->id + 1)) {
                return finger;
            }
        }
    }
    return NULL;
}

void set_id(ecs *world, const entity e, const int new_id) {
    if (!zox_has(e, ID)) {
        zox_log_error("Invalid zevice, no ID");
        return;
    }
    zox_muter(e, ID, id);
    id->value = new_id;
}

void finger_released(ecs *world, const entity e) {
    zox_muter(e, ZevicePointer, pointer);
    if (devices_get_pressed(pointer->value)) {
        devices_set_is_pressed(&pointer->value, 0);
        devices_set_released_this_frame(&pointer->value, 1);
        set_id(world, e, 0);
        global_any_fingers_down = 1; // one more frame
    }
}

void sdl_extract_touchscreen(
    ecs *world,
    const Children *children,
    const int2 touchscreen_size
) {
    touch_fingers_count = 0;
    touch_devices_count = SDL_GetNumTouchDevices();
    for (int i = 0; i < children->length; i++) {
        const entity zevice = children->value[i];
        if (!zox_valid(zevice) || !zox_has(zevice, Finger)) {
            continue;
        }
        const int zevice_id = zox_get_value(zevice, ID)
        if (zevice_id) {
            SDL_Finger* finger = find_finger(zevice_id);
            if (finger) {
                zox_muter(zevice, ZevicePointerPosition, zevicePointerPosition)
                zox_muter(zevice, ZevicePointerDelta, zevicePointerDelta)
                int2 finger_position = (int2) { (int) (finger->x * touchscreen_size.x), (int) (finger->y * touchscreen_size.y) };
                int2_flip_y(&finger_position, touchscreen_size);
                if (!int2_equals(zevicePointerPosition->value, int2_hidden)) {
                    const int2 delta_finger = int2_sub_int2_(finger_position, zevicePointerPosition->value);
                    zevicePointerDelta->value = delta_finger;
                }
                zevicePointerPosition->value = finger_position;
                global_any_fingers_down = 1;
                zox_logv(" > finger touching [%lu] fingerid [%i]", zevice, zevice_id)
            } else {
                finger_released(world, zevice);
                zox_logv(" - finger released [%lu] fingerid [%i]", zevice, zevice_id)
            }
        } else {
            // get unused finger! find a finger that isn't used yet
            SDL_Finger* finger = find_finger_unused(world, children);
            if (finger) {
                const int finger_id = finger->id + 1;
                set_id(world, zevice, finger_id);
                zox_muter(zevice, ZevicePointer, zevicePointer)
                zox_muter(zevice, ZevicePointerPosition, zevicePointerPosition)
                devices_set_pressed_this_frame(&zevicePointer->value, 1);
                devices_set_is_pressed(&zevicePointer->value, 1);
                int2 finger_position = (int2) { (int) (finger->x * touchscreen_size.x), (int) (finger->y * touchscreen_size.y) };
                int2_flip_y(&finger_position, touchscreen_size);
                zevicePointerPosition->value = finger_position;
                global_any_fingers_down = 1;
                zox_logv(" + finger touched [%lu] fingerid [%i]", zevice, finger_id)
            }
        }
    }
}
