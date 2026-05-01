static inline void zox_gpu_clear_viewport() {
    // glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

static inline void zox_gpu_set_clear_color(float3 c) {
    glClearColor(c.x, c.y, c.z, 1.0f);
}

static inline void zox_gpu_set_viewport(float2 position, float2 size) {
    glViewport(position.x, position.y, size.x, size.y);
}

/*static inline void clear_depth_buffer(uint fbo) {
    zox_gpu_bind_fbo(fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    zox_gpu_bind_fbo(0); // Bind the default framebuffer
}*/
