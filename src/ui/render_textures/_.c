#include "com/_.c"
#include "mat/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_render_textures(ecs* world) {
    zox_module(render_textures);
    zox_components_render_textures(world);
    zox_systems_render_textures(world);
    add_hook_spawn_prefabs(spawn_prefabs_render_textures);
    add_hook_load_shader(&spawn_materials_render_textures);
}
