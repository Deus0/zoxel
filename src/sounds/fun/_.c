

void dispose_sounds(ecs *world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_files_sounds();
}

#include "spawn.c"
#include "setting.c"
#include "terminal.c"
