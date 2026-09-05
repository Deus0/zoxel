// Settings

static inline void zox_gpu_enable_culling() {
    glEnable(GL_CULL_FACE);
}

static inline void zox_gpu_disable_culling() {
    glDisable(GL_CULL_FACE);
}

static inline void zox_gpu_set_depth_mask(byte is_depth_mask) {
    if (is_depth_mask) {
        glDepthMask(GL_TRUE);
    } else {
        glDepthMask(GL_FALSE);
    }
}

static inline void zox_gpu_enable_depth_test() {
    glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);
    glDepthFunc(GL_LEQUAL);
}

static inline void zox_gpu_disable_depth_test() {
    glDisable(GL_DEPTH_TEST);
}

static inline void zox_gpu_enable_blend() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static inline void zox_gpu_enable_blend2() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static inline void zox_gpu_disable_blend() {
    glDisable(GL_BLEND);
}

static inline void zox_gpu_enable_polyoffset() {
    glEnable(GL_POLYGON_OFFSET_FILL);
}

static inline void zox_gpu_disable_polyoffset() {
    glDisable(GL_POLYGON_OFFSET_FILL);
}

static inline void zox_gpu_polyoffset(float a, float b) {
    glPolygonOffset(a, b);
}

static inline void zox_gpu_enable_polyoffset_lines() {
    glEnable(GL_POLYGON_OFFSET_LINE);
    // GLenum error = glGetError();
    // printf("polygon offset line error: 0x%x\n", error);
}

static inline void zox_gpu_disable_polyoffset_lines() {
    glDisable(GL_POLYGON_OFFSET_LINE);
}
