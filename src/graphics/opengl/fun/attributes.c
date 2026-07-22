static inline void zox_gpu_disable_attribute(guint id) {
    glDisableVertexAttribArray(id);
}

static inline void zox_gpu_enable_attribute(guint id) {
    glEnableVertexAttribArray(id);
}

static inline void zox_gpu_attribute_divisor(guint id, guint value) {
    glVertexAttribDivisor(id, value);
}

static inline void zox_gpu_enable_attribute_float2(guint id) {
    zox_gpu_enable_attribute(id);
    glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
}

static inline void zox_gpu_set_attribute_floatx(guint id, const void* ptr, byte length) {
    glVertexAttribPointer(id, length, GL_FLOAT, GL_FALSE, 0, ptr);
}

static inline void zox_gpu_set_attribute_float2(guint id, const void* ptr) {
    glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, ptr);
}

static inline void zox_gpu_enable_set_attribute_float2(guint id, const void* ptr) {
    zox_gpu_enable_attribute(id);
    glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, ptr);
}

static inline void zox_gpu_enable_attribute_float3(guint id, const void* ptr) {
    zox_gpu_enable_attribute(id);
    glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, ptr);
}

static inline void zox_gpu_set_attribute_float3(guint id, const void* ptr) {
    glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, ptr);
}

static inline void zox_gpu_set_attribute_color(guint id, const void* ptr) {
    glVertexAttribPointer(id, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, ptr);
}
