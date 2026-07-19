static inline guint zox_gpu_create_buffer() {
    guint o;
    glGenBuffers(1, &o);
    return o;
}

static inline void zox_gpu_bind_buffer_element(guint v) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v);
}

static inline void zox_gpu_bind_buffer_array(guint v) {
    glBindBuffer(GL_ARRAY_BUFFER, v);
}

static inline void zox_gpu_set_buffer_element(const void* ptr, gsize length) {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, length, ptr, GL_STATIC_DRAW);
}

static inline void zox_gpu_set_buffer_array(const void* ptr, gsize length) {
    glBufferData(GL_ARRAY_BUFFER, length, ptr, GL_STATIC_DRAW);
}

static inline void zox_gpu_dispose_buffer(guint link) {
    if (link) glDeleteBuffers(1, &link);
}

static inline void zox_gpu_set_sub_buffer_float3(gsize length, const void* ptr) {
    glBufferSubData(GL_ARRAY_BUFFER, 0, length * sizeof(float3), ptr);
}

static inline void zox_gpu_set_sub_buffer_color(gsize length, const void* ptr) {
    glBufferSubData(GL_ARRAY_BUFFER, 0, length * sizeof(color), ptr);
}

static inline void opengl_enable_vertex_buffer(guint shader_index, guint vertex_buffer) {
    zox_gpu_bind_buffer_array(vertex_buffer);
    glEnableVertexAttribArray(shader_index);
    glVertexAttribPointer(shader_index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    zox_gpu_bind_buffer_array(0);
}

static inline void opengl_enable_uv_buffer(guint shader_index, guint uv_buffer) {
    zox_gpu_bind_buffer_array(uv_buffer);
    glEnableVertexAttribArray(shader_index);
    glVertexAttribPointer(shader_index, 2, GL_FLOAT, GL_FALSE,  0, 0);
    zox_gpu_bind_buffer_array(0);
}

static inline void opengl_enable_color_buffer(guint shader_index, guint color_buffer) {
    zox_gpu_bind_buffer_array(color_buffer);
    glEnableVertexAttribArray(shader_index);
    glVertexAttribPointer(shader_index, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    zox_gpu_bind_buffer_array(0);
}

static inline void zox_gpu_array_buffer_byte(guint shader_index, guint buffer) {
    zox_gpu_bind_buffer_array(buffer);
    glEnableVertexAttribArray(shader_index);
    glVertexAttribPointer(shader_index, 1, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    zox_gpu_bind_buffer_array(0);
}

static inline void zox_gpu_array_buffer(guint link, gsize length, byte byte_size, void* ptr) {
    zox_gpu_bind_buffer_array(link);
    zox_gpu_set_buffer_array(ptr, length * byte_size);
    zox_gpu_bind_buffer_array(0);
}

static inline void zox_gpu_element_buffer(guint link, gsize length, byte byte_size, void* ptr) {
    zox_gpu_bind_buffer_element(link);
    zox_gpu_set_buffer_element(ptr, length * byte_size);
    zox_gpu_bind_buffer_element(0);
}
