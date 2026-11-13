static inline void zox_gpu_set_clear_color(float4 clear) {
    glClearColor(clear.x, clear.y, clear.z, clear.w);
}

static inline void zox_gpu_clear_viewport() {
    // glClear(GL_COLOR_BUFFER_BIT);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

// clears only a rect of the screen: glClear ignores the viewport, so a camera drawing to part of
// the screen (split screen, tst model_views) would wipe the cameras drawn before it
static inline void zox_gpu_clear_rect(
    float2 position,
    float2 size)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(position.x, position.y, size.x, size.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}

static inline void zox_gpu_set_viewport(
    float2 position,
    float2 size)
{
    glViewport(position.x, position.y, size.x, size.y);
}
