// Textures

static inline void zox_gpu_bind_texture(guint id) {
    glBindTexture(GL_TEXTURE_2D, id);
}

static inline void zox_gpu_reset_texture() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

static inline void zox_gpu_set_texture_color_rgb(guint id, int2 size, const void* ptr) {
    zox_gpu_bind_texture(id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, ptr);
    zox_gpu_bind_texture(0);
}

static inline void zox_gpu_set_texture_color_rgba(guint id, int2 size, const void* ptr) {
    zox_gpu_bind_texture(id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
    zox_gpu_bind_texture(0);
}

static inline void zox_gpu_clear_texture_rgba(guint id) {
    // byte clear[4] = { 0, 0, 0, 0 }; // RGBA
    // glClearTexImage(id, 0, GL_RGBA, GL_UNSIGNED_BYTE, clear);
    zox_gpu_bind_texture(id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    zox_gpu_bind_texture(0);
}

static inline void zox_gpu_dispose_texture(guint id) {
    if (id) glDeleteTextures(1, &id);
}

// GL_NEAREST or GL_LINEAR
static inline guint spawn_gpu_texture_buffer() {
    int filter = GL_NEAREST;
    guint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    zox_gpu_bind_texture(id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    zox_gpu_reset_texture();
    return id;
}

// NOTE: for texture wrapping
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
