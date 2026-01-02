#include "vulkan.c"
#include "opengl.c"
#include "hooks.c"
#include "sdl_events.c"
#include "app_vulkan.c"
#include "app.c"

void dispose_apps_sdl(ecs *world, void *ctx) {
    (void) world;
    (void) ctx;
    // dispose_sdl_cursor();
    close_sdl_input();
    close_sdl_video();
    // dispose_mixer_pool();
}