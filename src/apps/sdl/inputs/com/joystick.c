#ifdef zox_sdl

zoxc(SdlJoystick, SDL_Joystick*);

void free_sdl_gamepad(SdlJoystick *ptr) {
    if (ptr->value) {
        zox_sdl_joystick_close(ptr->value);
    }
}

ECS_DTOR(SdlJoystick, ptr, {
    free_sdl_gamepad(ptr);
})

#endif
