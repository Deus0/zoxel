// Lines

static inline void zox_gpu_line_thickness(float thickness) {
    glLineWidth(thickness);
}

static inline void zox_gpu_render_lines(gsizei length) {
    glDrawArrays(GL_LINES, 0, length);
}

// one line per instance, 2 vertices each (glDrawArraysInstanced)
static inline void zox_gpu_render_lines_instanced(gsizei count) {
    glDrawArraysInstanced(GL_LINES, 0, 2, count);
}
