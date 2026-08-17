#include "file_shader.c"
#include "shader.c"

entity prefab_shader;
entity prefab_file_shader;

void spawn_prefabs_shaders(ecs *world) {
    //if (!using_gpu) {
        // shader_precision = 1;
        //zox_logw("Software Rendering Detected: Lowering Shader Precision.");
    //}
    prefab_shader = spawn_prefab_shader(world);
    prefab_file_shader = spawn_prefab_file_shader(world);
}
