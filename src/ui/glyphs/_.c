byte font_texture_min_size = 32;

#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "io/_.c"

void import_glyphs(ecs* world) {
    zox_module(glyphs);
    zox_define_components_zigels(world);
    zox_define_systems_zigels(world);
    add_hook_files_load(load_files_fonts);
    add_hook_spawn_prefabs(zox_spawn_prefabs_zigels);
}
