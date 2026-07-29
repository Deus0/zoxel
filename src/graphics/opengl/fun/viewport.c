static inline void zox_gpu_set_clear_color(float4 clear) {
    glClearColor(clear.x, clear.y, clear.z, clear.w);
}

static inline void zox_gpu_clear_viewport() {
    // glClear(GL_COLOR_BUFFER_BIT);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

static inline void zox_gpu_set_viewport(float2 position, float2 size) {
    glViewport(position.x, position.y, size.x, size.y);
}
