// Attributes

static inline void zox_gpu_disable_attribute(uint id) {
    glDisableVertexAttribArray(id);
}

static inline void zox_gpu_enable_attribute(uint id) {
    glEnableVertexAttribArray(id);
}

static inline void zox_gpu_attribute_divisor(uint id, byte value) {
    glVertexAttribDivisor(id, value);
}

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
