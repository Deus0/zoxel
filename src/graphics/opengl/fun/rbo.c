byte dbg_log_rbo = 0;

static inline guint zox_gpu_create_rbo() {
    guint id;
    glGenRenderbuffers(1, &id);
    if (dbg_log_rbo) {
        zox_log("Generated RBO [%i]", id);
    }
    return id;
}

static inline void zox_gpu_dispose_rbo(guint id) {
    if (!id) {
        return;
    }
    if (dbg_log_rbo) {
        zox_log("Disposing RBO [%i]", id);
    }
    glDeleteRenderbuffers(1, &id);
}

static inline void zox_gpu_set_rbo_size(guint id, int2 size) {
    if (!id) {
        zox_loge("[zox_gpu_set_rbo_size] Invalid RBO");
        return;
    }
#ifndef zox_gles2
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    if (dbg_log_rbo) {
        zox_log("Set RBO Size [%i]: [%ix%i]", id, size.x, size.y);
    }
#else
    zox_loge("RBO is disabled in gles2");
#endif
}

static inline void zox_gpu_link_fbo_rbo(guint fbo, guint rbo) {
    if (!fbo) {
        zox_loge("[zox_gpu_link_fbo_rbo] FBO Invalid");
        return;
    }
    if (!rbo) {
        zox_loge("[zox_gpu_link_fbo_rbo] RBO Invalid");
        return;
    }
    zox_gpu_bind_fbo(fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    zox_gpu_reset_fbo();
    if (dbg_log_rbo) {
        zox_log("Binded FBO [%i] to RBO [%i]", fbo, rbo);
    }
    /*if (!check_opengl_frame_buffer_status()) {
        zox_loge("connect_render_buffer_to_fbo error on fbo [%u]\n", fbo);
    }*/
}