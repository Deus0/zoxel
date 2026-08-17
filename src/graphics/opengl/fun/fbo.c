// Function to check if glGenFramebuffers is available
byte dbg_log_fbo = 0;

static inline byte check_opengl_frame_buffer_status() {
    // only enable when debugging
    if (!dbg_log_fbo) {
        return 1;
    }
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status) {
        case GL_FRAMEBUFFER_COMPLETE:
            // zox_log("Framebuffer COMPLETE");
            return 1;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            zox_loge("Framebuffer INCOMPLETE_ATTACHMENT");
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            zox_loge("Framebuffer INCOMPLETE_MISSING_ATTACHMENT");
            break;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            zox_loge("Framebuffer UNSUPPORTED");
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            zox_loge("Framebuffer INCOMPLETE_MULTISAMPLE");
            break;
        default:
            zox_loge("Framebuffer UNKNOWN STATUS [0x%04X]", status);
            break;
    }
    return 0;
}

static inline uint zox_gpu_create_fbo() {
    guint id;
    glGenFramebuffers(1, &id);
    if (dbg_log_fbo) {
        zox_log("Generated FBO [%i]", id);
    }
    return id;
}

static inline void zox_gpu_dispose_fbo(guint id) {
    if (!id) {
        return;
    }
    if (dbg_log_fbo) {
        zox_log("Disposing FBO [%i]", id);
    }
    glDeleteFramebuffers(1, &id);
}

static inline void zox_gpu_reset_fbo() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static inline void zox_gpu_bind_fbo(guint id) {
    if (dbg_log_fbo >= 2) {
        zox_log("Binding FBO [%i]", id);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    check_opengl_frame_buffer_status();
}

static inline void zox_gpu_fbo_to_texture(guint fbo, guint texture) {
    if (!fbo) {
        zox_loge("[zox_gpu_fbo_to_texture] FBO Invalid");
        return;
    }
    if (!texture) {
        zox_loge("[zox_gpu_fbo_to_texture] Texture Invalid");
        return;
    }
    zox_gpu_bind_fbo(fbo);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        texture,
        0);
    if (dbg_log_fbo) {
        zox_log("Binded FBO [%i] to Texture [%i]", fbo, texture);
    }
    check_opengl_frame_buffer_status();
    zox_gpu_reset_fbo();
}

extern byte apps_is_extension_supported(const char* name) ;

byte is_frame_buffer_supported() {
    return apps_is_extension_supported("GL_ARB_framebuffer_object");
    // Check if the necessary function is available
    /*if (GLEW_VERSION_3_0 || GLEW_ARB_framebuffer_object) {
        // Check if the function pointer is not NULL
        if (glGenFramebuffers != NULL) {
            return 1;
        } else {
            fprintf(stderr, "Function pointer for glGenFramebuffers is NULL\n");
            return 0;
        }
    } else {
        fprintf(stderr, "glGenFramebuffers is not available\n");
        return 0;
    }*/
}

byte test_frame_buffer() {
    if (!zox_get_safe_ubo_size()) {
        zox_logw("Frame Buffer not supported");
        return 1;
    }
    /*if (!is_frame_buffer_supported()) {
        zox_log(" ! frame buffer is unsupported.\n")
    } else {
        zox_log(" + frame buffer is supported.\n")
    }*/
    zox_logv("Uniform Buffer Size is [%i]", zox_get_safe_ubo_size());
    return 0;
}