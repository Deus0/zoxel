// Settings

static inline void zox_gpu_enable_culling() {
    glEnable(GL_CULL_FACE);
}

static inline void zox_gpu_disable_culling() {
    glDisable(GL_CULL_FACE);
}

static inline void zox_gpu_enable_depth_test() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // glDepthFunc(GL_LEQUAL);
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
