// Textures

byte dbg_gpu_texture = 0;

static inline void zox_gpu_bind_texture(guint id) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    if (dbg_gpu_texture >= 2) {
        zox_log("Bind Texture [%u]", id);
    }
}

static inline void zox_gpu_reset_texture() {
    glBindTexture(GL_TEXTURE_2D, 0);
    // glDeactiveTexture(GL_TEXTURE0);
}

static inline void zox_gpu_set_texture_color_rgb(guint id, int2 size, const void* ptr) {
    zox_gpu_bind_texture(id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        size.x,
        size.y,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        ptr);
    zox_gpu_bind_texture(0);
    if (dbg_gpu_texture) {
        zox_log("Bind Texture RGB [%u]: [%ix%i]", id, size.x, size.y);
    }
}

static inline void zox_gpu_set_texture_color_rgba(
    guint id,
    int2 size,
    const void* ptr)
{
    zox_gpu_bind_texture(id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        size.x,
        size.y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        ptr);
    zox_gpu_bind_texture(0);
    if (dbg_gpu_texture) {
        zox_log("Bind Texture RGBA [%u]: [%ix%i]", id, size.x, size.y);
    }
}

extern byte apps_is_extension_supported(const char* name);

// a camera's colour target can hold colours past 1 (doc/plans/hdr.md): gl 3, gles 3 with a float colour buffer
//  extension (webgl 2's), never gles2
static inline byte zox_gpu_hdr_supported() {
#ifdef zox_gles2
    return 0;
#else
    static byte supported = 2;
    if (supported == 2) {
        supported = opengl_mode != zox_opengl_es || apps_is_extension_supported("GL_EXT_color_buffer_float") ||
            apps_is_extension_supported("GL_EXT_color_buffer_half_float");
    }
    return supported;
#endif
}

// a camera's colour target, half floats: colours past 1 kept for the post pass' tonemap (doc/plans/hdr.md)
static inline void zox_gpu_set_texture_color_hdr(guint id, int2 size) {
#ifndef zox_gles2
    zox_gpu_bind_texture(id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
    zox_gpu_bind_texture(0);
#endif
}

static inline void zox_gpu_clear_texture_rgba(guint id) {
    // NOTE: This is required for clearing properly
    byte clear[4] = { 0, 0, 0, 0 }; // RGBA
    zox_gpu_set_texture_color_rgba(id, int2_one, clear);
    /*zox_gpu_bind_texture(id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        0,
        0,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL);
    zox_gpu_bind_texture(0);*/
}

static inline void zox_gpu_clear_texture_rgb(guint id) {
    zox_gpu_bind_texture(id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        0,
        0,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        NULL);
    zox_gpu_bind_texture(0);
}

static inline void zox_gpu_dispose_texture(guint id) {
    if (id) {
        glDeleteTextures(1, &id);
        zox_gpu_textures_live--;
    }
}

// GL_NEAREST or GL_LINEAR
static inline guint spawn_gpu_texture_buffer() {
    int filter = GL_NEAREST;
    guint id;
    glGenTextures(1, &id);
    zox_gpu_textures_live++;
    glBindTexture(GL_TEXTURE_2D, id);
    zox_gpu_bind_texture(id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    zox_gpu_reset_texture();
    if (dbg_gpu_texture) {
        zox_log("Spawned Texture [%u]", id);
    }
    return id;
}

// NOTE: for texture wrapping
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
