// --------------------------------------------------
// EGL
// --------------------------------------------------
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