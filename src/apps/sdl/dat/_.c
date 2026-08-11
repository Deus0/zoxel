// SDL_WINDOW_FULLSCREEN - fullscreen (exclusive)
// SDL_WINDOW_FULLSCREEN_DESKTOP - borderless windowed
// byte sdl_fullscreen_byte = (byte) SDL_WINDOW_FULLSCREEN_DESKTOP;
#ifdef zox_sdl3
byte sdl_fullscreen_byte = (byte) 1;
#else
byte sdl_fullscreen_byte = (byte) SDL_WINDOW_FULLSCREEN;
#endif
#include "opengl_states.c"
