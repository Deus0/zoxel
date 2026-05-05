#include "error.c"

// Frame Buffers

static inline uint zox_gpu_create_rbo() {
    uint id;
    glGenRenderbuffers(1, &id);
    return id;
}

static inline uint zox_gpu_create_fbo() {
    uint id;
    glGenFramebuffers(1, &id);
    return id;
}

static inline void zox_gpu_bind_fbo(uint fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

static inline void zox_gpu_link_fbo(uint fbo, uint texture) {
    zox_gpu_bind_fbo(fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
#ifdef zoxel_catch_opengl_errors
    if (check_opengl_error_unlogged()) {
        zox_log_error(" > failed fbo to texture [%i] : [%i]\n", fbo, texture);
    }
#endif
    zox_gpu_bind_fbo(0);
}

static inline void zox_gpu_link_fbo_rbo(uint fbo, uint rbo) {
    zox_gpu_bind_fbo(fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if (!check_opengl_frame_buffer_status()) {
        zox_log_error("connect_render_buffer_to_fbo error on fbo [%u]\n", fbo);
    }
    zox_gpu_bind_fbo(0);
}

static inline void zox_gpu_dispose_fbo(uint id) {
    if (id) glDeleteFramebuffers(1, &id);
}

// RBOs

static inline void zox_gpu_set_rbo_size(uint id, int2 size) {
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

static inline void zox_gpu_dispose_rbo(uint id) {
    if (id) glDeleteRenderbuffers(1, &id);
}

// Shaders

static inline void zox_gpu_dispose_shader(uint id) {
    glDeleteShader(id);
}

// Triangles

static inline void zox_gpu_render(uint length) {
    glDrawElements(GL_TRIANGLES, length, GL_UNSIGNED_INT, NULL);
}

static inline void zox_gpu_render_triangles_instanced(uint indicies, uint length) {
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
#include "geometry.c"
#include "compute.c"
#include "ubo.c"
#include "frame_buffer.c"
#include "debug.c"
#include "glew.c"
