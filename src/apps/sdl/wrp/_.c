// todo: move anything with EcS out of wrapper!
#ifdef zox_sdl3
    #include "sdl3.c"
    #include "sdl3_input.c"
    #include "sdl3_sound.c"
    #include "sdl3_events.c"
#else
    #include "sdl2.c"
    #include "sdl2_input.c"
    #include "sounds.c"
    #include "sdl2_events.c"
#endif
#ifdef zox_sdl_mixer
    #ifdef zox_sdl3
        #include "sdl3_mixer.c"
    #else
        #include "sdl2_mixer.c"
    #endif
#else
    #include "no_mixer.c"
#endif

#include "monitor.c"
#include "screen.c"
#include "window.c"
#include "opengl.c"
#include "debug.c"
#include "video.c"
#include "virtual_keyboard.c"
#include "orientation.c"

byte initialize_sdl(const char* name) {
    zox_logv("Initializing Video");
    if (initialize_video() == EXIT_FAILURE) {
        zox_loge("[initialize_video] failed");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

byte apps_is_extension_supported(const char* name) {
    return SDL_GL_ExtensionSupported(name);
}