void zox_dispose_material(uint value) {
    if (value) glDeleteProgram(value);
}

void zox_gpu_dispose_texture(uint value) {
    if (value) glDeleteTextures(1, &value);
}

void zox_gpu_bind_buffer_element(uint v) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v);
}

void zox_gpu_bind_buffer_array(uint v) {
    glBindBuffer(GL_ARRAY_BUFFER, v);
}

void zox_gpu_set_buffer_element(const void* ptr, uint length) {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, length, ptr, GL_STATIC_DRAW);
}

void zox_gpu_set_buffer_array(const void* ptr, uint length) {
    glBufferData(GL_ARRAY_BUFFER, length, ptr, GL_STATIC_DRAW);
}

void zox_gpu_dispose_buffer(uint link) {
    if (link) glDeleteBuffers(1, &link);
}

#include "buffer.c"
#include "error.c"
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
