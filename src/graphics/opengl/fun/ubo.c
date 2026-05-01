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
