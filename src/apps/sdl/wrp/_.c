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

void set_sdl_app_settings(const char* name) {
#ifdef sdl3
   /*SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, name);
    char app_id[256];
    snprintf(app_id, sizeof(app_id), "org.zox.%s", name);
    SDL_SetAppMetadataProperty(
        SDL_PROP_APP_METADATA_IDENTIFIER_STRING,
        app_id);*/
#endif
}

byte initialize_sdl(const char* name) {
    set_sdl_app_settings(name);
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