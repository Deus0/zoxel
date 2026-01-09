#ifndef zoxm_sdl
#define zoxm_sdl

#include "com/_.c"
#include "dat/_.c"
#include "tst/_.c"
#include "wrapper/_.c"
#include "set/_.c"
#include "pre/_.c"
#include "inputs/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Sdl)
    // Sdl settings
    SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
    // disable_virtual_keyboard();
    define_components_sdl(world);
    define_systems_sdl(world);
    // init
    zox_module_dispose(dispose_apps_sdl);
    // hooks
    add_to_update_loop(update_sdl);
    add_to_post_update_loop(app_update_gpu);
    add_hook_terminal_command(process_terminal_sdl);
    add_hook_spawn_prefabs(spawn_prefabs_sdl);
    zox_import_module(SdlInputs);

    zox_log("HHas VK Support? %i", can_virtual_keyboard());
zox_end_module(Sdl)

#endif
