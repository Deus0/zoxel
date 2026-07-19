static inline void zox_gpu_enable_points() {
    glEnable(GL_PROGRAM_POINT_SIZE);
}

static inline void zox_gpu_render_points(gsizei length) {
    glDrawArrays(GL_POINTS, 0, length);
}

static inline void zox_gpu_render_points_instanced(gsizei length) {
    glDrawArraysInstanced(GL_POINTS, 0, 1, length);
}
