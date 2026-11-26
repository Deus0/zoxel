byte dbg_log_rbo = 0;

static inline void check_opengl_rbo(guint rbo) {
    if (!dbg_log_rbo) {
        return;
    }
    GLint bound_rbo = 0;
    GLint width = 0;
    GLint height = 0;
    GLint internal_format = 0;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &bound_rbo);
    if ((guint) bound_rbo != rbo) {
        zox_loge(
            "RBO [%i] not bound, currently bound [%i]",
            rbo,
            bound_rbo
        );
        return;
    }
    glGetRenderbufferParameteriv(
        GL_RENDERBUFFER,
        GL_RENDERBUFFER_WIDTH,
        &width
    );
    glGetRenderbufferParameteriv(
        GL_RENDERBUFFER,
        GL_RENDERBUFFER_HEIGHT,
        &height
    );
    glGetRenderbufferParameteriv(
        GL_RENDERBUFFER,
        GL_RENDERBUFFER_INTERNAL_FORMAT,
        &internal_format
    );
    zox_log(
        "RBO [%i]: [%ix%i] format [0x%04X]",
        rbo,
        width,
        height,
        internal_format
    );
}

// a render camera's depth target: a renderbuffer on gles2; else a depth texture (the outline pass reads it) and an
//  outline mask texture beside it (rg32ui, colour attachment 1: a model's outline id and its window depth's bits,
//  integer so blending never touches it), the mask kept
//  here by the depth texture's id so every path that makes, sizes, links or frees one does both
#ifndef zox_gles2
guint* zox_gl_rbo_masks = NULL;
uint zox_gl_rbo_masks_length = 0;

static inline guint zox_gpu_rbo_mask(guint rbo) {
    return rbo < zox_gl_rbo_masks_length ? zox_gl_rbo_masks[rbo] : 0;
}

static inline void zox_gl_texture_target(guint id) {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
#else
static inline guint zox_gpu_rbo_mask(guint rbo) {
    return 0;
}
#endif

static inline guint zox_gpu_create_rbo() {
    guint id;
#ifndef zox_gles2
    glGenTextures(1, &id);
    guint mask;
    glGenTextures(1, &mask);
    if (id >= zox_gl_rbo_masks_length) {
        const uint length = id * 2 + 8;
        zox_gl_rbo_masks = realloc(zox_gl_rbo_masks, length * sizeof(guint));
        memset(zox_gl_rbo_masks + zox_gl_rbo_masks_length, 0, (length - zox_gl_rbo_masks_length) * sizeof(guint));
        zox_gl_rbo_masks_length = length;
    }
    zox_gl_rbo_masks[id] = mask;
#else
    glGenRenderbuffers(1, &id);
#endif
    zox_gpu_rbos_live++;
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
#ifndef zox_gles2
    guint mask = zox_gpu_rbo_mask(id);
    if (mask) {
        glDeleteTextures(1, &mask);
        zox_gl_rbo_masks[id] = 0;
    }
    glDeleteTextures(1, &id);
#else
    glDeleteRenderbuffers(1, &id);
#endif
    zox_gpu_rbos_live--;
}

static inline void zox_gpu_set_rbo_size(guint id, int2 size) {
    if (!id) {
        zox_loge("[zox_gpu_set_rbo_size] Invalid RBO");
        return;
    }
#ifndef zox_gles2
    zox_gl_texture_target(id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    const guint mask = zox_gpu_rbo_mask(id);
    if (mask) {
        zox_gl_texture_target(mask);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32UI, size.x, size.y, 0, GL_RG_INTEGER, GL_UNSIGNED_INT, NULL);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
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
#ifndef zox_gles2
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rbo, 0);
    const guint mask = zox_gpu_rbo_mask(rbo);
    if (mask) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mask, 0);
    }
    // the mask only drawn to while outlined models draw (zox_gpu_outline_mask)
    const GLenum buffers[2] = { GL_COLOR_ATTACHMENT0, GL_NONE };
    glDrawBuffers(2, buffers);
#else
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
#endif
    if (dbg_log_rbo) {
        zox_log("Binded FBO [%i] to RBO [%i]", fbo, rbo);
    }
    check_opengl_frame_buffer_status();
    zox_gpu_reset_fbo();
}

// outlines (doc/plans/outlines.md): not on gles2 (no depth textures, one colour output)
static inline byte zox_gpu_outlines_supported() {
#ifndef zox_gles2
    return 1;
#else
    return 0;
#endif
}

// the bound render camera's mask drawn to (outlined models' draws) or not (everything else)
static inline void zox_gpu_outline_mask(byte on) {
#ifndef zox_gles2
    const GLenum buffers[2] = { GL_COLOR_ATTACHMENT0, on ? GL_COLOR_ATTACHMENT1 : GL_NONE };
    glDrawBuffers(2, buffers);
#endif
}

// the bound render camera's mask cleared: no models
static inline void zox_gpu_clear_outline_mask() {
#ifndef zox_gles2
    zox_gpu_outline_mask(1);
    const GLuint zero[4] = { 0, 0, 0, 0 };
    glClearBufferuiv(GL_COLOR, 1, zero);
    zox_gpu_outline_mask(0);
#endif
}

// a texture on a unit past 0 (the outline pass' depth and mask), unit 0 left active
static inline void zox_gpu_bind_texture_unit(byte unit, guint id) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
    glActiveTexture(GL_TEXTURE0);
}
