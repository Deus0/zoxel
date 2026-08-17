#ifndef zox_render_textures
#define zox_render_textures

#include "com/_.c"
#include "mat/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

zox_begin_module(RenderTextures) {
    define_components_render_textures(world);
    define_systems_render_textures(world);
    add_hook_spawn_prefabs(spawn_prefabs_render_textures);
    add_hook_load_shader(&spawn_materials_render_textures);
} zox_end_module(RenderTextures)

#endif
