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
    zox_gpu_fbos_live++;
    if (dbg_log_fbo) {
        zox_log("Generated FBO [%i]", id);
    }
    return id;
}

// each fbo's colour texture (zox_gpu_fbo_to_texture), for its mips after it draws (zox_gpu_fbo_mips)
#define zox_gl_fbo_links_max 64
guint2 zox_gl_fbo_links[zox_gl_fbo_links_max];     // x the fbo, y its texture

static inline void zox_gl_fbo_link(guint fbo, guint texture) {
    int free_slot = -1;
    for (int i = 0; i < zox_gl_fbo_links_max; i++) {
        if (zox_gl_fbo_links[i].x == fbo) {
            zox_gl_fbo_links[i].y = texture;
            return;
        }
        if (free_slot < 0 && !zox_gl_fbo_links[i].x) {
            free_slot = i;
        }
    }
    if (free_slot >= 0) {
        zox_gl_fbo_links[free_slot] = (guint2) { fbo, texture };
    }
}

// bloom's mips of the fbo's texture, from what it drew (doc/plans/hdr.md)
static inline void zox_gpu_fbo_mips(guint fbo) {
    for (int i = 0; i < zox_gl_fbo_links_max; i++) {
        if (zox_gl_fbo_links[i].x == fbo && zox_gl_fbo_links[i].y) {
            glBindTexture(GL_TEXTURE_2D, zox_gl_fbo_links[i].y);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            return;
        }
    }
}

static inline void zox_gpu_dispose_fbo(guint id) {
    if (!id) {
        return;
    }
    for (int i = 0; i < zox_gl_fbo_links_max; i++) {
        if (zox_gl_fbo_links[i].x == id) {
            zox_gl_fbo_links[i] = (guint2) { 0, 0 };
        }
    }
    if (dbg_log_fbo) {
        zox_log("Disposing FBO [%i]", id);
    }
    glDeleteFramebuffers(1, &id);
    zox_gpu_fbos_live--;
}

// hdr output is vulkan's (graphics/dat/hdr_output.c): egl's hdr colour spaces are patchy, gl stays sdr
static inline void zox_gpu_set_hdr_output(byte asked) { (void) asked; }
static inline byte zox_gpu_hdr_output_supported() { return 0; }

static inline void zox_gpu_reset_fbo() {
    glBindFramebuffer(GL_FRAMEBUFFER, zox_gpu_default_fbo);
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
    zox_gl_fbo_link(fbo, texture);
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