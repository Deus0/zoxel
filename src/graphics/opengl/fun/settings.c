// Settings

static inline void zox_gpu_enable_culling() {
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
}

static inline void zox_gpu_disable_culling() {
    glDisable(GL_CULL_FACE);
}

static inline void zox_gpu_enable_depth_test() {
    glEnable(GL_DEPTH_TEST);
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
    // glDepthMask(GL_FALSE);
    // glDepthFunc(GL_LEQUAL);
    // glDepthMask(GL_FALSE);
}

static inline void zox_gpu_disable_blend() {
    glDisable(GL_BLEND);
}
