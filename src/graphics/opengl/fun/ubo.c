// UBOs

static inline void zox_gpu_ubo_set_matricies(guint id, gsizei length, const void* ptr) {
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, length * sizeof(float4x4), ptr);
}

// as set, its storage orphaned first (fresh, its capacity): the draw before can still read the old, no stall waiting
//  for it (filling one buffer again while the gpu reads it made the driver wait, tst vox_instances_stress)
static inline void zox_gpu_ubo_stream_matricies(guint id, gsizei length, gsizei capacity, const void* ptr) {
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, capacity * sizeof(float4x4), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, length * sizeof(float4x4), ptr);
}

static inline void zox_gpu_ubo_initialize_matricies(guint id, guint binding_point, gsizei length) {
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(float4x4) * length, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, id);
}

// a program's uniform block reads the buffer bound at binding (glBindBufferBase); lost when the program relinks
static inline void zox_gpu_ubo_block_binding(guint program, guint block_index, guint binding) {
    if (block_index != GL_INVALID_INDEX) {
        glUniformBlockBinding(program, block_index, binding);
    }
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
