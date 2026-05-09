#include "settings.c"
#include "chunk.c"
#include "block_vox.c"
#include "debug.c"
#include "terrain.c"
#include "terminal.c"
#include "set.c"
#include "game.c"

void module_dispose_terrain(ecs *world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_hook_spawn_blocks();
}
