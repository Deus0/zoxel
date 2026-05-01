#ifndef zoxm_sdl
#define zoxm_sdl

#include "dat/_.c"
#include "set/_.c"
#include "com/_.c"
#include "tst/_.c"
#include "wrapper/_.c"
#include "pre/_.c"
#include "inputs/_.c"
#include "fun/_.c"
#include "sys/_.c"

byte channels_count = 2;
uint channel_sample_rate = 44100;

void initialize_sounds() {
    if (nosounds) {
        return;
    }

    if (initialize_sdl_mixer(channel_sample_rate, channels_count) == EXIT_SUCCESS) {
        audio_enabled = 1;
    } else {
        zox_log_error("[initialize_sdl_mixer] failed");
    }
}

zox_begin_module(Sdl)
    // disable_virtual_keyboard();
    define_components_sdl(world);
    define_systems_sdl(world);

    // hooks
    zox_module_dispose(dispose_apps_sdl);
    add_to_update_loop(update_sdl);
    add_to_post_update_loop(app_update_gpu);
    add_hook_terminal_command(process_terminal_sdl);
    add_hook_spawn_prefabs(spawn_prefabs_sdl);
    zox_import_module(SdlInputs);

    initialize_sounds();

    // Sdl settings
    SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");

    zox_logv("Virtual Keyboard Support? %i", can_virtual_keyboard());

zox_end_module(Sdl)

#endif
