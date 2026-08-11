#ifndef zoxm_sdl
#define zoxm_sdl

byte use_sdl_audio = 0;
byte channels_count = 2;
uint channel_sample_rate = 44100;
#include "dat/_.c"
#include "wrp/_.c"
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "inputs/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "tst/_.c"

void initialize_sounds() {
    if (nosounds) {
        return;
    }
    if (initialize_sdl_mixer(channel_sample_rate, channels_count) == EXIT_SUCCESS) {
        audio_enabled = 1;
    } else {
        zox_loge("[initialize_sdl_mixer] failed");
    }
}

zox_begin_module(Sdl) {
    define_components_sdl(world);
    define_systems_sdl(world);
    zox_module_dispose(dispose_apps_sdl);
    add_to_post_update_loop(app_update_gpu);
    add_hook_terminal_command(process_terminal_sdl);
    add_hook_spawn_prefabs(spawn_prefabs_sdl);
    initialize_sounds();
    zox_import_module(SdlInputs);
    add_to_update_loop(update_sdl);
    // Setting idk
    // SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
} zox_end_module(Sdl);

#endif
