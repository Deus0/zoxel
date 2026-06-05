// #define zox_debug_log_element_raycasting
int touch_devices_count = 0;
int touch_fingers_count = 0;

SDL_Finger* find_finger(int finger_id) {
    for (int j = 0; j < touch_devices_count; j++) {
        SDL_TouchID touchscreen_id = SDL_GetTouchDevice(j);
        int fingers_count = SDL_GetNumTouchFingers(touchscreen_id);
        touch_fingers_count += fingers_count;
        for (int k = 0; k < fingers_count; k++) {
            SDL_Finger *finger = SDL_GetTouchFinger(touchscreen_id, k);
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
