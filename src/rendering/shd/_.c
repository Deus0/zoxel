#ifndef zoxm_shaders
#define zoxm_shaders

byte is_log_shaders = 0;
#define directory_shaders "shaders"
int files_count_shaders = 0;
// NOTE: For some reason it  breaks on lower precision than high
byte shader_precision_level = 2;
entity *files_shaders;
string_hashmap *files_hashmap_shaders;
// zox_tag(Shader)
zoxc_arrayd(ShaderSource, char);
#include "set/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/io.c"

void dispose_shaders(ecs *world, void *ctx) {
    dispose_files_shaders();
}

zox_begin_module(Shaders) {
    zoxd_arrayd(ShaderSource);
    // zoxd_tag(Shader)
    zox_module_dispose(dispose_shaders);
    // add_hook_files_load(load_files_shaders);
    add_hook_spawn_prefabs(spawn_prefabs_shaders);
} zox_end_module(Shaders);

#endif
