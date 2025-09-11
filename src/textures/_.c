#ifndef zoxm_textures
#define zoxm_textures

#include "dat/_.c"
#include "com/_.c"
#include "gen/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "tst/test_texture.c"
#include "fun/sdl_textures.c"
#include "io/_.c"
#include "dbg/_.c"

// todo: rename Textures as TextureLinks, and Textures to Textures again
zox_begin_module(Textures)
    define_components_textures(world);
    define_systems_textures(world);
    add_hook_spawn_prefabs(spawn_prefabs_textures_core);
    zox_import_module(TexturesIO);
zox_end_module(Textures)

#endif
