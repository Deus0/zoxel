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

static inline void zox_gpu_clear_texture_rgba(uint id) {
    // byte clear[4] = { 0, 0, 0, 0 }; // RGBA
    // glClearTexImage(id, 0, GL_RGBA, GL_UNSIGNED_BYTE, clear);
    zox_gpu_bind_texture(id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    zox_gpu_bind_texture(0);
}

static inline void zox_gpu_dispose_texture(uint id) {
    if (id) glDeleteTextures(1, &id);
}


static inline void opengl_bind_texture(uint texture_buffer) {
    glBindTexture(GL_TEXTURE_2D, texture_buffer);
}

static inline void opengl_reset_texture() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

static inline uint spawn_gpu_texture_buffer() {
    const int texture_type = GL_NEAREST; // GL_NEAREST | GL_LINEAR
    uint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_type);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}
