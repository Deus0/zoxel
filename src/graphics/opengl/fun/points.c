static inline void zox_gpu_enable_points() {
    if (!zox_gpu_is_es()) {
        glEnable(GL_PROGRAM_POINT_SIZE);
    }
}

// triangles from their vertex shader alone, no vertex or index buffer (a fullscreen pass, gl_VertexID)
static inline void zox_gpu_render_triangles(gsizei vertices) {
    glDrawArrays(GL_TRIANGLES, 0, vertices);
}

static inline void zox_gpu_render_points(gsizei length) {
    glDrawArrays(GL_POINTS, 0, length);
}

static inline void zox_gpu_render_points_instanced(gsizei length) {
    glDrawArraysInstanced(GL_POINTS, 0, 1, length);
}
