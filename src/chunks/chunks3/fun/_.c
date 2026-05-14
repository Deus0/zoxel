#include "lods.c"
#include "bounds.c"
#include "voctree.c"
#include "positioning.c"
#include "ground.c"

void module_dispose_chunks3(ecs *world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_hook_on_destroyed_VoxelNode();
}
