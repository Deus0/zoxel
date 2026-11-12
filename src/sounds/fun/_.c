#include "spatial.c"
#include "../mix/mixer.c"
#include "spawn.c"
#include "effects.c"
#include "setting.c"
#include "terminal.c"

void dispose_sounds(ecs *world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_files_sounds();
    dispose_mixer();
}
