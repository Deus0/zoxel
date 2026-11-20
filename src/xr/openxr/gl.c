// --------------------------------------------------
// XR Graphics: OpenGL ES (EGL's context, the engine draws with it)
// --------------------------------------------------
//  - the swapchain images are textures, attached to an eye's fbo (its depth a renderbuffer of its own)
//  - drawn y flipped (the images are read top down), so the triangles wind clockwise while an eye draws
#define xr_graphics_extension XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME
#define xr_graphics_flip_y 1
static EGLDisplay egl_display = EGL_NO_DISPLAY;
static EGLConfig egl_config = NULL;
static EGLContext egl_context = EGL_NO_CONTEXT;
static EGLSurface egl_surface = EGL_NO_SURFACE;

static int egl_setup() {
    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (egl_display == EGL_NO_DISPLAY) {
        zox_loge("[XR] eglGetDisplay failed");
        return 0;
    }
    EGLint major;
    EGLint minor;
    if (!eglInitialize(egl_display, &major, &minor)) {
        zox_loge("[XR] eglInitialize failed");
        return 0;
    }
    zox_log("[XR] EGL version: %d.%d", major, minor);
    const EGLint config_attributes[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    EGLint config_count = 0;
    if (!eglChooseConfig(egl_display, config_attributes, &egl_config, 1, &config_count) || config_count == 0) {
        zox_loge("[XR] eglChooseConfig failed");
        return 0;
    }
    const EGLint context_attributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    eglBindAPI(EGL_OPENGL_ES_API);
    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, context_attributes);
    if (egl_context == EGL_NO_CONTEXT) {
        zox_loge("[XR] eglCreateContext failed: 0x%x", eglGetError());
        return 0;
    }
    const EGLint surface_attributes[] = {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1,
        EGL_NONE
    };
    egl_surface = eglCreatePbufferSurface(egl_display, egl_config, surface_attributes);
    if (egl_surface == EGL_NO_SURFACE) {
        zox_loge("[XR] eglCreatePbufferSurface failed: 0x%x", eglGetError());
        return 0;
    }
    if (!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context)) {
        zox_loge("[XR] eglMakeCurrent failed: 0x%x", eglGetError());
        return 0;
    }
    zox_log("[XR] EGL context created: display=%p config=%p context=%p", (void *)egl_display, (void *)egl_config, (void *)egl_context);
    zox_log("[XR] GL version: %s", glGetString(GL_VERSION));
    return 1;
}

static void egl_shutdown() {
    if (egl_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(
            egl_display,
            EGL_NO_SURFACE,
            EGL_NO_SURFACE,
            EGL_NO_CONTEXT
        );
    }
    if (egl_surface != EGL_NO_SURFACE) {
        eglDestroySurface(egl_display, egl_surface);
        egl_surface = EGL_NO_SURFACE;
    }
    if (egl_context != EGL_NO_CONTEXT) {
        eglDestroyContext(egl_display, egl_context);
        egl_context = EGL_NO_CONTEXT;
    }
    if (egl_display != EGL_NO_DISPLAY) {
        eglTerminate(egl_display);
        egl_display = EGL_NO_DISPLAY;
    }
}


static void xr_debug_gl_state(const char *label) {
    GLint fbo = 0;
    GLint viewport[4] = { 0 };
    GLint scissor[4] = { 0 };
    GLint draw_fbo = 0;
    GLint read_fbo = 0;
    GLboolean color_mask[4] = { 0 };
    GLboolean depth_mask = 0;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &draw_fbo);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read_fbo);
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetIntegerv(GL_SCISSOR_BOX, scissor);

    glGetBooleanv(GL_COLOR_WRITEMASK, color_mask);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depth_mask);

    zox_log(
        "[XR GL] %s"
        " fbo=%i"
        " draw=%i"
        " read=%i"
        " viewport=%i,%i %ix%i"
        " scissor=%i,%i %ix%i"
        " scissor_enabled=%i"
        " color_mask=%i,%i,%i,%i"
        " depth_mask=%i"
        " blend=%i"
        " depth=%i"
        " cull=%i",
        label,
        fbo,
        draw_fbo,
        read_fbo,
        viewport[0],
        viewport[1],
        viewport[2],
        viewport[3],
        scissor[0],
        scissor[1],
        scissor[2],
        scissor[3],
        glIsEnabled(GL_SCISSOR_TEST),
        color_mask[0],
        color_mask[1],
        color_mask[2],
        color_mask[3],
        depth_mask,
        glIsEnabled(GL_BLEND),
        glIsEnabled(GL_DEPTH_TEST),
        glIsEnabled(GL_CULL_FACE)
    );
}

static int xr_graphics_begin() {
    if (!egl_setup()) {
        zox_loge("[XR] EGL setup failed");
        return 0;
    }
    return 1;
}

static void xr_graphics_end() {
    egl_shutdown();
}

// OpenXR asks for its graphics requirements before the session
static int xr_graphics_system() {
    XrGraphicsRequirementsOpenGLESKHR requirements = {
        .type = XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR
    };
    PFN_xrGetOpenGLESGraphicsRequirementsKHR get_requirements = NULL;
    XrResult result = xrGetInstanceProcAddr(xr_instance, "xrGetOpenGLESGraphicsRequirementsKHR",
        (PFN_xrVoidFunction *) &get_requirements);
    if (!xr_check_result(result, "xrGetInstanceProcAddr(xrGetOpenGLESGraphicsRequirementsKHR)") || !get_requirements) {
        return 0;
    }
    result = get_requirements(xr_instance, system_id, &requirements);
    if (!xr_check_result(result, "xrGetOpenGLESGraphicsRequirementsKHR")) {
        return 0;
    }
    zox_log("[XR] GL versions: %u.%u to %u.%u, actual: %s",
        XR_VERSION_MAJOR(requirements.minApiVersionSupported), XR_VERSION_MINOR(requirements.minApiVersionSupported),
        XR_VERSION_MAJOR(requirements.maxApiVersionSupported), XR_VERSION_MINOR(requirements.maxApiVersionSupported),
        (const char *) glGetString(GL_VERSION));
    return 1;
}

static const void* xr_graphics_binding() {
    static XrGraphicsBindingOpenGLESAndroidKHR binding;
    binding = (XrGraphicsBindingOpenGLESAndroidKHR) {
        .type = XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR,
        .display = egl_display,
        .config = egl_config,
        .context = egl_context
    };
    return &binding;
}

// rgba8 if offered, else the runtime's first
static int64_t xr_graphics_format(const int64_t* formats, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        if (formats[i] == GL_RGBA8) {
            return formats[i];
        }
    }
    return formats[0];
}

static void set_camera_buffers_size(GLuint fbo, GLuint rbo, int2 size) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// each eye's fbo and depth, the swapchain's images attached as they're acquired
static int xr_graphics_eyes_begin() {
    for (int i = 0; i < 2; i++) {
        glGenFramebuffers(1, &xr_eyes[i].fbo);
        glGenRenderbuffers(1, &xr_eyes[i].rbo);
        set_camera_buffers_size(xr_eyes[i].fbo, xr_eyes[i].rbo, xr_config_to_image_size(xr_eyes[i].config));
    }
    return 1;
}

static void xr_graphics_eyes_end() {
    for (int i = 0; i < 2; i++) {
        glDeleteRenderbuffers(1, &xr_eyes[i].rbo);
        glDeleteFramebuffers(1, &xr_eyes[i].fbo);
    }
}

// the eye's next image acquired and attached to its fbo (tst/cube.c draws with these alone)
static int xr_gl_eye_bind(byte i) {
    xr_eye* eye = &xr_eyes[i];
    if (!xr_swapchain_acquire(eye)) {
        return 0;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, eye->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, eye->images[eye->image_index].image, 0);
    return 1;
}

static void xr_gl_eye_unbind(byte i) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    xr_swapchain_release(&xr_eyes[i]);
}

static int xr_graphics_eye_begin(byte i) {
    if (!xr_gl_eye_bind(i)) {
        return 0;
    }
    glFrontFace(GL_CW);     // y flipped into the swapchain images (fun/eye_render.c), so the winding too
    return 1;
}

static void xr_graphics_eye_end(byte i) {
    glFrontFace(GL_CCW);
    xr_gl_eye_unbind(i);
}
