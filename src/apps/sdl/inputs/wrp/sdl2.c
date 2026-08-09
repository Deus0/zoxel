void zox_sdl_joystick_close(SDL_Joystick *joystick) {
    SDL_JoystickClose(joystick);
}

const char* zox_sdl_gamepad_name(SDL_GameController* ptr) {
    return SDL_GameControllerName(ptr);
}

byte sdl_mouse_button_pressed(uint buttons, uint button) {
    return buttons & SDL_BUTTON(button);
}

int zox_num_gamepads() {
    return SDL_NumJoysticks();
}

SDL_JoystickID zox_get_gamepad(int index) {
    return (SDL_JoystickID) index;
}

byte zox_is_gamepad(SDL_JoystickID index) {
    return SDL_IsGameController(index);
}

// SDL2
#define zox_sdl_gamepad_open SDL_GameControllerOpen
#define zox_sdl_gamepad_close SDL_GameControllerClose
#define zox_sdl_gamepad_connected SDL_GameControllerGetAttached


// SDL2
uint sdl_get_touch_device_count() {
    return (uint) SDL_GetNumTouchDevices();
}

SDL_TouchID sdl_get_touch_device(uint index) {
    return SDL_GetTouchDevice((int) index);
}

uint sdl_get_touch_finger_count(SDL_TouchID device) {
    return (uint) SDL_GetNumTouchFingers(device);
}

SDL_Finger* sdl_get_touch_finger(SDL_TouchID device, uint index) {
    return SDL_GetTouchFinger(device, (int) index);
}

byte sdl_get_gamepad_button(SDL_GameController* controller, SDL_GameControllerButton button) {
    return SDL_GameControllerGetButton(controller, button);
}

int16_t sdl_get_gamepad_axis(SDL_GameController* controller, SDL_GameControllerAxis axis) {
    return SDL_GameControllerGetAxis(controller, axis);
}