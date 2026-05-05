static inline void zox_gpu_enable_points() {
    glEnable(GL_PROGRAM_POINT_SIZE);
}

static inline void zox_gpu_render_points(uint length) {
    glDrawArrays(GL_POINTS, 0, length);
}

static inline void zox_gpu_render_points_instanced(uint length) {
    glDrawArraysInstanced(GL_POINTS, 0, 1, length);
}