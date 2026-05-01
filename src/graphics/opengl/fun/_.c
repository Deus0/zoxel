#include "error.c"

static inline void zox_gpu_clear_viewport() {
    // glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

// Settings

static inline void zox_gpu_enable_culling() {
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
}

static inline void zox_gpu_disable_culling() {
    glDisable(GL_CULL_FACE);
}

static inline void zox_gpu_enable_depth_test() {
    glEnable(GL_DEPTH_TEST);
}

static inline void zox_gpu_disable_depth_test() {
    glDisable(GL_DEPTH_TEST);
}

static inline void zox_gpu_enable_blend() {
    glEnable(GL_BLEND);
}

static inline void zox_gpu_disable_blend() {
    glDisable(GL_BLEND);
}

// Materials

static inline void zox_dispose_material(uint value) {
    if (value) glDeleteProgram(value);
}

static inline uint zox_gpu_get_material_property(uint id, const char* name) {
    return glGetUniformLocation(id, name);
}

static inline uint zox_gpu_get_material_attribute(uint id, const char* name) {
    return glGetAttribLocation(id, name);
}

// Textures

static inline void zox_gpu_bind_texture(uint id) {
    glBindTexture(GL_TEXTURE_2D, id);
}

static inline void zox_gpu_set_texture_color_rgb(uint id, int2 size, const void* ptr) {
    zox_gpu_bind_texture(id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, ptr);
    zox_gpu_bind_texture(0);
}

static inline void zox_gpu_set_texture_color_rgba(uint id, int2 size, const void* ptr) {
    zox_gpu_bind_texture(id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
    zox_gpu_bind_texture(0);
}

static inline void zox_gpu_dispose_texture(uint id) {
    if (id) glDeleteTextures(1, &id);
}

// Buffers

static inline uint zox_gpu_create_buffer() {
    uint o;
    glGenBuffers(1, &o);
    return o;
}

static inline void zox_gpu_bind_buffer_element(uint v) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v);
}

static inline void zox_gpu_bind_buffer_array(uint v) {
    glBindBuffer(GL_ARRAY_BUFFER, v);
}

static inline void zox_gpu_set_buffer_element(const void* ptr, uint length) {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, length, ptr, GL_STATIC_DRAW);
}

static inline void zox_gpu_set_buffer_array(const void* ptr, uint length) {
    glBufferData(GL_ARRAY_BUFFER, length, ptr, GL_STATIC_DRAW);
}

static inline void zox_gpu_dispose_buffer(uint link) {
    if (link) glDeleteBuffers(1, &link);
}

static inline void zox_gpu_set_sub_buffer_float3(uint length, const void* ptr) {
    glBufferSubData(GL_ARRAY_BUFFER, 0, length * sizeof(float3), ptr);
}

static inline void zox_gpu_set_sub_buffer_color(uint length, const void* ptr) {
    glBufferSubData(GL_ARRAY_BUFFER, 0, length * sizeof(color), ptr);
}

// Frame Buffers

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

// UBOs

static inline void zox_gpu_ubo_set_matricies(uint id, uint length, const void* ptr) {
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, length * sizeof(float4x4), ptr);
}

static inline void zox_gpu_ubo_initialize_matricies(uint id, uint binding_point, uint length) {
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(float4x4) * length, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, id);
}

static inline void zox_gpu_ubo_reset() {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

int zox_get_safe_ubo_size() {
    int size = 0;
#ifndef zox_disable_ubos
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &size);
    // mat4 = 16 floats = 64 bytes
    size = size / (sizeof(float) * 16);
    if (size >= 10000) {
        size = 10000;
    }
#endif
    return size;
}

// Shaders

static inline void zox_gpu_dispose_shader(uint id) {
    glDeleteShader(id);
}

// Attributes

static inline void zox_gpu_enable_attribute_float2(uint id) {
    glEnableVertexAttribArray(id);
    glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

static inline void zox_gpu_set_attribute_float2(uint id, const void* ptr) {
    glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, ptr);   // (GLFloat*) ?
}

static inline void zox_gpu_enable_set_attribute_float2(uint id, const void* ptr) {
    glEnableVertexAttribArray(id);
    glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, ptr);
}

static inline void zox_gpu_enable_attribute_float3(uint id) {
    glEnableVertexAttribArray(id);
    glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

static inline void zox_gpu_set_attribute_float3(uint id, const void* ptr) {
    glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, ptr);
}

static inline void zox_gpu_set_attribute_color(uint id, const void* ptr) {
    glVertexAttribPointer(id, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, ptr);
}

// Triangles

static inline void zox_gpu_render(uint length) {
    glDrawElements(GL_TRIANGLES, length, GL_UNSIGNED_INT, NULL);
}

static inline void zox_gpu_render_triangles_instanced(uint indicies, uint length) {
    // zox_gpu_render_triangles(meshIndicies->length, render_count);
    glDrawElementsInstanced(GL_TRIANGLES, indicies, GL_UNSIGNED_INT, 0, length);
}

// Lines

static inline void zox_gpu_line_thickness(const float thickness) {
    glLineWidth(thickness);
}

static inline void zox_gpu_render_lines(uint length) {
    glDrawArrays(GL_LINES, 0, length);
}

// Points

static inline void zox_gpu_render_points(uint length) {
    glDrawArrays(GL_POINTS, 0, length);
}

static inline void zox_gpu_render_points_instanced(uint length) {
    glDrawArraysInstanced(GL_POINTS, 0, 1, length);
}

#include "buffers.c"

#include "shaders.c"
#include "extensions.c"
#include "geometry.c"
#include "compute.c"
#include "ubo.c"
#include "frame_buffer.c"
#include "material.c"
#include "uniform.c"
#include "line.c"
#include "debug.c"
#include "glew.c"
