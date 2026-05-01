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

static inline uint zox_gpu_get_material_uniform(uint id, const char* name) {
    return glGetUniformBlockIndex(id, name);
}

static inline void zox_gpu_material(uint material) {
    glUseProgram(material);
}

static inline void zox_disable_material() {
    glUseProgram(0);
}

static inline void zox_gpu_reset_mesh() {
    zox_gpu_bind_buffer_element(0);
    zox_gpu_bind_buffer_array(0);
}

static inline uint2 spawn_gpu_mesh_buffers() {
    uint2 mesh = (uint2) { zox_gpu_create_buffer(), zox_gpu_create_buffer() };
    return mesh;
}
