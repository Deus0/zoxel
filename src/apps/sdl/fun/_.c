#include "vulkan.c"
#include "opengl.c"
#include "hooks.c"
#include "events.c"
#include "app.c"
#include "app_vulkan.c"
#include "app_opengl.c"

void dispose_apps_sdl(ecs *world, void *ctx) {
    (void) world;
    (void) ctx;
    // dispose_sdl_cursor();
    close_sdl_input();
    close_sdl_video();
    // dispose_mixer_pool();
}
