#ifndef zoxm_shaders
#define zoxm_shaders

byte is_log_shaders = 0;
#define directory_shaders "shaders"

int files_count_shaders = 0;
byte shader_precision_level = 2;

entity *files_shaders;
string_hashmap *files_hashmap_shaders;
entity prefab_file_shader;

// zox_tag(Shader)
zoxc_arrayd(ShaderSource, char)
#include "pre/file_shader.c"
#include "fun/_.c"

void dispose_shaders(ecs *world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_files_shaders();
}

void spawn_prefabs_shaders(ecs* world) {
    if (!using_gpu) {
        shader_precision_level = 1;
        zox_log("Software Rendering Detected: Lowering Shader Precision.");
    }
    prefab_file_shader = spawn_prefab_file_shader(world);
}

zox_begin_module(Shaders)
    zoxd_arrayd(ShaderSource);
    // zoxd_tag(Shader)
    zox_module_dispose(dispose_shaders);
    // add_hook_files_load(load_files_shaders);
    add_hook_spawn_prefabs(spawn_prefabs_shaders);
zox_end_module(Shaders)

#endif
