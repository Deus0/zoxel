// todo: move anything with EcS out of wrapper!
#ifdef zox_sdl3
    #include "sdl3.c"
    #include "sdl3_input.c"
    #include "sdl3_sound.c"
#else
    #include "sdl2.c"
    #include "sdl2_input.c"
    #include "sounds.c"
#endif

#include "monitor.c"
#include "screen.c"
#include "window.c"
#include "opengl.c"
#include "debug.c"
#include "video.c"
#include "mixer.c"
#include "virtual_keyboard.c"
#include "orientation.c"

byte apps_is_extension_supported(const char* name) {
    return SDL_GL_ExtensionSupported(name);
}