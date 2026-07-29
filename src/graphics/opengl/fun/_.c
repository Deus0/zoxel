#include "error.c"
#include "ubo.c"
#include "fbo.c"
#include "rbo.c"

// Shaders

// Triangles

static inline void zox_gpu_render3(gsizei length) {
    if (zox_render_lines) {
        glDrawElements(GL_LINES, length, GL_UNSIGNED_INT, 0);
    } else {
        glDrawElements(GL_TRIANGLES, length, GL_UNSIGNED_INT, 0); // NULL);
    }
}

static inline void zox_gpu_render(gsizei length) {
    glDrawElements(GL_TRIANGLES, length, GL_UNSIGNED_INT, 0);
}

static inline void zox_gpu_render_as_lines(gsizei length) {
    glDrawElements(GL_LINES, length, GL_UNSIGNED_INT, 0);
}

static inline void zox_gpu_render_triangles_instanced(gsizei indicies, gsizei length) {
    // zox_gpu_render_triangles(meshIndicies->length, render_count);
    glDrawElementsInstanced(GL_TRIANGLES, indicies, GL_UNSIGNED_INT, 0, length);
}

#include "buffers.c"
#include "texture.c"
#include "settings.c"
#include "viewport.c"
#include "material.c"
#include "uniform.c"
#include "points.c"
#include "lines.c"
#include "attributes.c"
#include "shaders.c"
#include "extensions.c"
#include "compute.c"
#include "geometry.c"
#include "debug.c"
#include "glew.c"
