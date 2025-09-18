#include "line.c"

void spawn_shaders_lines3D(ecs* world) {
    if (render_backend == zox_render_backend_opengl) {
        initialize_shader_line3D(world);
    }
}