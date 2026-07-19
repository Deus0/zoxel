// Materials

static inline void zox_dispose_material(guint value) {
    if (value) glDeleteProgram(value);
}

static inline gint zox_gpu_get_material_property(guint id, const char* name) {
    return glGetUniformLocation(id, name);
}

static inline gint zox_gpu_get_material_attribute(guint id, const char* name) {
    return glGetAttribLocation(id, name);
}

static inline guint zox_gpu_get_material_uniform(guint id, const char* name) {
    return glGetUniformBlockIndex(id, name);
}

static inline void zox_gpu_material(guint material) {
    glUseProgram(material);
}

static inline void zox_disable_material() {
    glUseProgram(0);
}

static inline void zox_gpu_reset_mesh() {
    zox_gpu_bind_buffer_element(0);
    zox_gpu_bind_buffer_array(0);
}

static inline guint2 spawn_gpu_mesh_buffers() {
    guint2 result;
    glGenBuffers(2, &result.x);
    return result;
}

/*static inline guint2 spawn_gpu_mesh_buffers() {
    return (guint2) { zox_gpu_create_buffer(), zox_gpu_create_buffer() };
}*/
