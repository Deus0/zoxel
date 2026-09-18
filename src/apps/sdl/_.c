
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

void import_sdl(ecs* world) {
    zox_module(sdl);
    zox_components_sdl(world);
    zox_systems_sdl(world);
    zox_module_dispose(dispose_apps_sdl);
    add_hook_terminal_command(process_terminal_sdl);
    add_hook_spawn_prefabs(spawn_prefabs_sdl);
    initialize_sounds();
    zox_add_module(sdl_inputs);
    // Setting idk
    // SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
    if (is_on_phosh()) {
        zox_log("Phosh Detected. Disabling Decor.");
        disable_apps_decor = 1;
        fullscreen = 1;
    }
}