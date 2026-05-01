// Lines

static inline void zox_gpu_line_thickness(float thickness) {
    glLineWidth(thickness);
}

static inline void zox_gpu_render_lines(uint length) {
    glDrawArrays(GL_LINES, 0, length);
}
