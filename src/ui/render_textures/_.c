#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_render_textures(ecs* world) {
    zox_module(render_textures);
    zox_components_render_textures(world);
    zox_systems_render_textures(world);
    add_hook_spawn_prefabs(spawn_prefabs_render_textures);
#ifdef zox_tests
    zox_tsts_render_textures(world);
#endif
}
