#ifdef zox_sdl3
    typedef SDL_Gamepad zox_sdl_gamepad;
    typedef SDL_GamepadButton zox_sdl_gamepad_button;
    typedef SDL_GamepadAxis zox_sdl_gamepad_axis;

    #include "sdl3_keys.c"
    #include "sdl3.c"
#else
    typedef SDL_GameController zox_sdl_gamepad;
    typedef SDL_GameControllerButton zox_sdl_gamepad_button;
    typedef SDL_GameControllerAxis zox_sdl_gamepad_axis;

    #include "sdl2_keys.c"
    #include "sdl2.c"
#endif