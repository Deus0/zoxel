void zox_sdl_joystick_close(SDL_Joystick* ptr) {
    SDL_CloseJoystick(ptr);
}

const char* zox_sdl_gamepad_name(SDL_Gamepad* ptr) {
    return SDL_GetGamepadName(ptr);
}

byte sdl_mouse_button_pressed(uint buttons, uint button) {
    return buttons & SDL_BUTTON_MASK(button);
}

int zox_num_gamepads() {
    int count;
    SDL_JoystickID* gamepads = SDL_GetGamepads(&count);
    SDL_free(gamepads);
    return count;
}

SDL_JoystickID zox_get_gamepad(int index) {
    int count;
    SDL_JoystickID* gamepads = SDL_GetGamepads(&count);
    if (!gamepads || index >= count) {
        SDL_free(gamepads);
        return 0;
    }
    SDL_JoystickID result = gamepads[index];
    SDL_free(gamepads);
    return result;
}

byte zox_is_gamepad(SDL_JoystickID i) {
    return SDL_IsGamepad(i);
}

// SDL3
#define zox_sdl_gamepad_open SDL_OpenGamepad
#define zox_sdl_gamepad_close SDL_CloseGamepad
#define zox_sdl_gamepad_connected SDL_GamepadConnected

uint sdl_get_touch_device_count() {
    int count = 0;
    SDL_TouchID* devices = SDL_GetTouchDevices(&count);
    SDL_free(devices);
    return (uint) count;
}

SDL_TouchID sdl_get_touch_device(uint index) {
    int count = 0;
    SDL_TouchID* devices = SDL_GetTouchDevices(&count);
    if (!devices || index >= (uint) count) {
        SDL_free(devices);
        return 0;
    }
    SDL_TouchID result = devices[index];
    SDL_free(devices);
    return result;
}

uint sdl_get_touch_finger_count(SDL_TouchID device) {
    int count = 0;
    SDL_Finger** fingers = SDL_GetTouchFingers(device, &count);
    SDL_free(fingers);
    return (uint) count;
}

SDL_Finger* sdl_get_touch_finger(SDL_TouchID device, uint index) {
    int count = 0;
    SDL_Finger** fingers = SDL_GetTouchFingers(device, &count);
    if (!fingers || index >= (uint) count) {
        SDL_free(fingers);
        return NULL;
    }
    SDL_Finger* result = fingers[index];
    SDL_free(fingers);
    return result;
}

byte sdl_get_gamepad_button(SDL_Gamepad* controller, SDL_GamepadButton button) {
    return SDL_GetGamepadButton(controller, button);
}

Sint16 sdl_get_gamepad_axis(SDL_Gamepad* controller, SDL_GamepadAxis axis) {
    return SDL_GetGamepadAxis(controller, axis);
}