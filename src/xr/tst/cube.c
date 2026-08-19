// --------------------------------------------------
// Includes
// --------------------------------------------------

#include <jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengles2.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>
#include <stdarg.h>

static void zox_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    __android_log_vprint(ANDROID_LOG_INFO, "Zoxel", format, args);
    va_end(args);
}

static void zox_loge(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    __android_log_vprint(ANDROID_LOG_ERROR, "Zoxel", format, args);
    va_end(args);
}

#define SDL_Log(...) zox_log(__VA_ARGS__)
#define SDL_LogError(category, ...) zox_loge(__VA_ARGS__)


// --------------------------------------------------
// Globals
// --------------------------------------------------

static XrInstance xr_instance = XR_NULL_HANDLE;
static XrSystemId system_id;
static XrSession session;
static XrSpace space;
static XrViewConfigurationView views_cfg[2];
static XrSwapchain swapchain[2];
static XrSwapchainImageOpenGLESKHR *images[2];
static uint32_t image_count[2];
static int running;
static int session_running;
static EGLDisplay egl_display = EGL_NO_DISPLAY;
static EGLConfig egl_config = NULL;
static EGLContext egl_context = EGL_NO_CONTEXT;
static EGLSurface egl_surface = EGL_NO_SURFACE;

static int xr_check_result(XrResult result, const char *name) {
    if (XR_FAILED(result)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] %s failed: %d", name, result);
        return 0;
    }
    SDL_Log("[XR] %s", name);
    return 1;
}


// --------------------------------------------------
// Math
// --------------------------------------------------

typedef struct { float x, y, z; } float3;
typedef struct { float x, y, z, w; } float4;

static float random_float(float min, float max) {
    return min + (float)rand() / (float)RAND_MAX * (max - min);
}

static float4 euler_to_quaternion(float3 e) {
    const float cx = cosf(e.x * 0.5f);
    const float sx = sinf(e.x * 0.5f);
    const float cy = cosf(e.y * 0.5f);
    const float sy = sinf(e.y * 0.5f);
    const float cz = cosf(e.z * 0.5f);
    const float sz = sinf(e.z * 0.5f);

    return (float4){
        sx * cy * cz - cx * sy * sz,
        cx * sy * cz + sx * cy * sz,
        cx * cy * sz - sx * sy * cz,
        cx * cy * cz + sx * sy * sz
    };
}

static void matrix_multiply(
    float *out,
    const float *a,
    const float *b
) {
    float result[16];

    for (int column = 0; column < 4; column++) {
        for (int row = 0; row < 4; row++) {
            result[column * 4 + row] =
                a[0 * 4 + row] * b[column * 4 + 0] +
                a[1 * 4 + row] * b[column * 4 + 1] +
                a[2 * 4 + row] * b[column * 4 + 2] +
                a[3 * 4 + row] * b[column * 4 + 3];
        }
    }

    memcpy(out, result, sizeof(result));
}

static void transform_matrix(float *m, float3 p, float4 q, float3 s) {
    const float x = q.x, y = q.y, z = q.z, w = q.w;
    m[0] = (1 - 2*(y*y + z*z)) * s.x;
    m[1] = (2*(x*y + z*w)) * s.x;
    m[2] = (2*(x*z - y*w)) * s.x;
    m[3] = 0;

    m[4] = (2*(x*y - z*w)) * s.y;
    m[5] = (1 - 2*(x*x + z*z)) * s.y;
    m[6] = (2*(y*z + x*w)) * s.y;
    m[7] = 0;

    m[8] = (2*(x*z + y*w)) * s.z;
    m[9] = (2*(y*z - x*w)) * s.z;
    m[10] = (1 - 2*(x*x + y*y)) * s.z;
    m[11] = 0;

    m[12] = p.x;
    m[13] = p.y;
    m[14] = p.z;
    m[15] = 1;
}

static void view_matrix(float *m, XrPosef p) {
    const float x = -p.orientation.x;
    const float y = -p.orientation.y;
    const float z = -p.orientation.z;
    const float w = p.orientation.w;

    const float xx = x * x;
    const float yy = y * y;
    const float zz = z * z;
    const float xy = x * y;
    const float xz = x * z;
    const float yz = y * z;
    const float wx = w * x;
    const float wy = w * y;
    const float wz = w * z;

    const float r00 = 1 - 2 * (yy + zz);
    const float r01 = 2 * (xy - wz);
    const float r02 = 2 * (xz + wy);
    const float r10 = 2 * (xy + wz);
    const float r11 = 1 - 2 * (xx + zz);
    const float r12 = 2 * (yz - wx);
    const float r20 = 2 * (xz - wy);
    const float r21 = 2 * (yz + wx);
    const float r22 = 1 - 2 * (xx + yy);

    m[0] = r00; m[1] = r10; m[2] = r20; m[3] = 0;
    m[4] = r01; m[5] = r11; m[6] = r21; m[7] = 0;
    m[8] = r02; m[9] = r12; m[10] = r22; m[11] = 0;

    m[12] = -(r00 * p.position.x + r01 * p.position.y + r02 * p.position.z);
    m[13] = -(r10 * p.position.x + r11 * p.position.y + r12 * p.position.z);
    m[14] = -(r20 * p.position.x + r21 * p.position.y + r22 * p.position.z);
    m[15] = 1;
}

static float *create_projection_matrix(float *m, XrFovf f, float n, float z) {
    const float l = tanf(f.angleLeft);
    const float r = tanf(f.angleRight);
    const float d = tanf(f.angleDown);
    const float u = tanf(f.angleUp);
    memset(m, 0, 16 * sizeof(float));
    m[0] = 2.0f / (r - l);
    m[5] = 2.0f / (u - d);
    m[8] = (r + l) / (r - l);
    m[9] = (u + d) / (u - d);
    m[10] = -(z + n) / (z - n);
    m[11] = -1.0f;
    m[14] = -(2.0f * z * n) / (z - n);
    return m;
}

// --------------------------------------------------
// EGL
// --------------------------------------------------

static int egl_setup(void) {
    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (egl_display == EGL_NO_DISPLAY) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] eglGetDisplay failed");
        return 0;
    }
    EGLint major;
    EGLint minor;
    if (!eglInitialize(egl_display, &major, &minor)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] eglInitialize failed");
        return 0;
    }
    SDL_Log("[XR] EGL version: %d.%d", major, minor);
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
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] eglChooseConfig failed");
        return 0;
    }
    const EGLint context_attributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    eglBindAPI(EGL_OPENGL_ES_API);
    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, context_attributes);
    if (egl_context == EGL_NO_CONTEXT) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] eglCreateContext failed: 0x%x", eglGetError());
        return 0;
    }
    const EGLint surface_attributes[] = {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1,
        EGL_NONE
    };
    egl_surface = eglCreatePbufferSurface(egl_display, egl_config, surface_attributes);
    if (egl_surface == EGL_NO_SURFACE) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] eglCreatePbufferSurface failed: 0x%x", eglGetError());
        return 0;
    }
    if (!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] eglMakeCurrent failed: 0x%x", eglGetError());
        return 0;
    }
    SDL_Log("[XR] EGL context created: display=%p config=%p context=%p", (void *)egl_display, (void *)egl_config, (void *)egl_context);
    SDL_Log("[XR] GL version: %s", glGetString(GL_VERSION));
    return 1;
}

static void egl_shutdown(void) {
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

// --------------------------------------------------
// OpenGL
// --------------------------------------------------

static GLuint make_shader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    return s;
}

static GLuint make_program(void) {
    const char *vs =
        "#version 300 es\n"
        "layout(location=0) in vec3 p;\n"
        "layout(location=1) in vec3 c;\n"
        "uniform mat4 vp;\n"
        "out vec3 col;\n"
        "void main(){gl_Position=vp*vec4(p,1.0);col=c;}";
    const char *fs =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec3 col;\n"
        "out vec4 o;\n"
        "void main(){o=vec4(col,1.0);}";
    GLuint p = glCreateProgram();
    GLuint v = make_shader(GL_VERTEX_SHADER, vs);
    GLuint f = make_shader(GL_FRAGMENT_SHADER, fs);
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}

// --------------------------------------------------
// Cubes
// --------------------------------------------------

#define CUBE_COUNT 1024
static float cube_scale_min = 0.1f;
static float cube_scale_max = 0.369f;
static float3 cube_min = (float3) { -64, -4, -64 };
static float3 cube_max = (float3) { 64, 32, 64 };
static float3 cube_positions[CUBE_COUNT];
static float3 cube_rotations[CUBE_COUNT];
static float3 cube_scales[CUBE_COUNT];

static const float cube[] = {
    -1,-1,-1, 1,0,0, 1,-1,-1, 1,0,0, 1,1,-1, 1,0,0,
    -1,-1,-1, 1,0,0, 1,1,-1, 1,0,0, -1,1,-1, 1,0,0,
    -1,-1,1, 0,1,0, 1,1,1, 0,1,0, 1,-1,1, 0,1,0,
    -1,-1,1, 0,1,0, -1,1,1, 0,1,0, 1,1,1, 0,1,0,
    -1,-1,-1, 0,0,1, -1,1,-1, 0,0,1, -1,1,1, 0,0,1,
    -1,-1,-1, 0,0,1, -1,1,1, 0,0,1, -1,-1,1, 0,0,1,
    1,-1,-1, 1,1,0, 1,-1,1, 1,1,0, 1,1,1, 1,1,0,
    1,-1,-1, 1,1,0, 1,1,1, 1,1,0, 1,1,-1, 1,1,0,
    -1,1,-1, 0,1,1, 1,1,-1, 0,1,1, 1,1,1, 0,1,1,
    -1,1,-1, 0,1,1, 1,1,1, 0,1,1, -1,1,1, 0,1,1,
    -1,-1,-1, 1,0,1, -1,-1,1, 1,0,1, 1,-1,1, 1,0,1,
    -1,-1,-1, 1,0,1, 1,-1,1, 1,0,1, 1,-1,-1, 1,0,1
};

static void spawn_cubes(void) {
    srand((unsigned int)time(NULL));
    for (int i = 0; i < CUBE_COUNT; i++) {
        cube_positions[i] = (float3) {
            random_float(cube_min.x, cube_max.x),
            random_float(cube_min.y, cube_max.y),
            random_float(cube_min.z, cube_max.z)
        };
        cube_rotations[i] = (float3) {
            random_float(0, 6.28f),
            random_float(0, 6.28f),
            random_float(0, 6.28f)
        };
        float scale = random_float(cube_scale_min, cube_scale_max);
        cube_scales[i] = (float3) { scale, scale, scale };
    }
}

// --------------------------------------------------
// Rendering
// --------------------------------------------------

static float3 sky = (float3) { 0.2f, 0.4f, 0.8f };

static void render_cube(
    const float *vp,
    float3 position,
    float3 euler,
    float3 scale,
    GLuint program,
    GLuint vbo
) {
    float4 rotation = euler_to_quaternion(euler);
    float model[16];
    transform_matrix(model, position, rotation, scale);
    float mvp[16];
    matrix_multiply(mvp, vp, model);
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "vp"), 1, GL_FALSE, mvp);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

static void render_camera(
    const XrView *view,
    int width,
    int height,
    GLuint program,
    GLuint vbo
) {
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glClearColor(sky.x, sky.y, sky.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float projection[16];
    float view_matrix_[16];
    float vp[16];

    create_projection_matrix(projection, view->fov, 0.05f, 100.0f);
    view_matrix(view_matrix_, view->pose);
    matrix_multiply(vp, projection, view_matrix_);

    for (int i = 0; i < CUBE_COUNT; i++) {
        cube_rotations[i].x += 0.003f;
        cube_rotations[i].y += 0.004f;
        render_cube(
            vp,
            cube_positions[i],
            cube_rotations[i],
            cube_scales[i],
            program,
            vbo
        );
    }
}

// --------------------------------------------------
// XR
// --------------------------------------------------

static int zox_xr_init(void) {
    SDL_Log("[XR] Initializing");
    JNIEnv *env = (JNIEnv *)SDL_GetAndroidJNIEnv();
    if (!env) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] SDL_GetAndroidJNIEnv failed");
        return 0;
    }
    JavaVM *java_vm = NULL;
    jint jni_result = (*env)->GetJavaVM(env, &java_vm);
    if (jni_result != JNI_OK || !java_vm) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] GetJavaVM failed");
        return 0;
    }
    jobject activity = (jobject)SDL_GetAndroidActivity();
    if (!activity) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] SDL_GetAndroidActivity failed");
        return 0;
    }
    PFN_xrInitializeLoaderKHR initialize_loader = NULL;
    XrResult result = xrGetInstanceProcAddr(
        XR_NULL_HANDLE,
        "xrInitializeLoaderKHR",
        (PFN_xrVoidFunction *)&initialize_loader
    );
    if (!xr_check_result(result, "xrGetInstanceProcAddr(xrInitializeLoaderKHR)")) {
        return 0;
    }
    if (!initialize_loader) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] xrInitializeLoaderKHR unavailable");
        return 0;
    }
    XrLoaderInitInfoAndroidKHR loader_info = {
        .type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR,
        .next = NULL,
        .applicationVM = java_vm,
        .applicationContext = activity
    };
    result = initialize_loader((const XrLoaderInitInfoBaseHeaderKHR *)&loader_info);
    if (!xr_check_result(result, "xrInitializeLoaderKHR")) {
        return 0;
    }
    uint32_t extension_count = 0;
    result = xrEnumerateInstanceExtensionProperties(NULL, 0, &extension_count, NULL);
    if (!xr_check_result(result, "xrEnumerateInstanceExtensionProperties(count)")) {
        return 0;
    }
    XrExtensionProperties *extensions = calloc(
        extension_count,
        sizeof(XrExtensionProperties)
    );
    if (!extensions) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] Extension allocation failed");
        return 0;
    }
    for (uint32_t i = 0; i < extension_count; i++) {
        extensions[i].type = XR_TYPE_EXTENSION_PROPERTIES;
    }
    uint32_t returned_extension_count = 0;
    result = xrEnumerateInstanceExtensionProperties(
        NULL,
        extension_count,
        &returned_extension_count,
        extensions
    );
    if (!xr_check_result(result, "xrEnumerateInstanceExtensionProperties")) {
        free(extensions);
        return 0;
    }
    int found_android = 0;
    int found_opengles = 0;
    for (uint32_t i = 0; i < returned_extension_count; i++) {
        if (strcmp(
            extensions[i].extensionName,
            XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME
        ) == 0) {
            found_android = 1;
        }
        if (strcmp(
            extensions[i].extensionName,
            XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME
        ) == 0) {
            found_opengles = 1;
        }
    }
    free(extensions);
    if (!found_android) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] Android extension unavailable");
        return 0;
    }
    if (!found_opengles) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[XR] OpenGL ES extension unavailable");
        return 0;
    }
    const char *enabled_extensions[] = {
        XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME,
        XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME
    };
    XrInstanceCreateInfoAndroidKHR android_info = {
        .type = XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR,
        .next = NULL,
        .applicationVM = java_vm,
        .applicationActivity = activity
    };
    XrInstanceCreateInfo create_info = {
        .type = XR_TYPE_INSTANCE_CREATE_INFO,
        .next = &android_info,
        .applicationInfo = {
            .applicationName = "Zoxel",
            .applicationVersion = 1,
            .engineName = "Zoxel",
            .engineVersion = 1,
            .apiVersion = XR_MAKE_VERSION(1, 0, 0)
        },
        .enabledApiLayerCount = 0,
        .enabledApiLayerNames = NULL,
        .enabledExtensionCount = 2,
        .enabledExtensionNames = enabled_extensions
    };
    result = xrCreateInstance(&create_info, &xr_instance);
    if (!xr_check_result(result, "xrCreateInstance")) {
        xr_instance = XR_NULL_HANDLE;
        return 0;
    }
    return 1;
}

static void zox_xr_shutdown(void) {
    if (xr_instance != XR_NULL_HANDLE) {
        xrDestroyInstance(xr_instance);
        xr_instance = XR_NULL_HANDLE;
        SDL_Log("[XR] Instance destroyed");
    }
}

static int xr_setup_system(void) {
    SDL_Log("[XR] Setting up system");

    XrSystemGetInfo system_info = {
        .type = XR_TYPE_SYSTEM_GET_INFO,
        .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY
    };

    XrResult result = xrGetSystem(
        xr_instance,
        &system_info,
        &system_id
    );

    if (!xr_check_result(result, "xrGetSystem")) {
        return 0;
    }

    XrGraphicsRequirementsOpenGLESKHR requirements = {
        .type = XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR
    };

    PFN_xrGetOpenGLESGraphicsRequirementsKHR get_requirements = NULL;

    result = xrGetInstanceProcAddr(
        xr_instance,
        "xrGetOpenGLESGraphicsRequirementsKHR",
        (PFN_xrVoidFunction *)&get_requirements
    );

    if (!xr_check_result(
        result,
        "xrGetInstanceProcAddr(xrGetOpenGLESGraphicsRequirementsKHR)"
    )) {
        return 0;
    }

    if (!get_requirements) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "[XR] OpenGL ES requirements function missing"
        );
        return 0;
    }

    result = get_requirements(
        xr_instance,
        system_id,
        &requirements
    );

    if (!xr_check_result(
        result,
        "xrGetOpenGLESGraphicsRequirementsKHR"
    )) {
        return 0;
    }

    SDL_Log(
        "[XR] GL min version: %u.%u",
        XR_VERSION_MAJOR(requirements.minApiVersionSupported),
        XR_VERSION_MINOR(requirements.minApiVersionSupported)
    );

    SDL_Log(
        "[XR] GL max version: %u.%u",
        XR_VERSION_MAJOR(requirements.maxApiVersionSupported),
        XR_VERSION_MINOR(requirements.maxApiVersionSupported)
    );

    SDL_Log(
        "[XR] GL actual: %s",
        (const char *)glGetString(GL_VERSION)
    );

    return 1;
}

static int xr_setup_views(void) {
    uint32_t view_count = 0;

    XrResult result = xrEnumerateViewConfigurationViews(
        xr_instance,
        system_id,
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        0,
        &view_count,
        NULL
    );

    if (!xr_check_result(
        result,
        "xrEnumerateViewConfigurationViews(count)"
    )) {
        return 0;
    }

    SDL_Log("[XR] View count: %u", view_count);

    if (view_count != 2) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "[XR] Expected 2 views, got %u",
            view_count
        );
        return 0;
    }

    for (uint32_t i = 0; i < view_count; i++) {
        views_cfg[i].type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
        views_cfg[i].next = NULL;
    }

    result = xrEnumerateViewConfigurationViews(
        xr_instance,
        system_id,
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        view_count,
        &view_count,
        views_cfg
    );

    return xr_check_result(
        result,
        "xrEnumerateViewConfigurationViews"
    );
}

static int xr_setup_session(void) {
    SDL_Log(
        "[XR] EGL display=%p config=%p context=%p",
        (void *)egl_display,
        (void *)egl_config,
        (void *)egl_context
    );

    XrGraphicsBindingOpenGLESAndroidKHR binding = {
        .type = XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR,
        .next = NULL,
        .display = egl_display,
        .config = egl_config,
        .context = egl_context
    };

    XrSessionCreateInfo session_info = {
        .type = XR_TYPE_SESSION_CREATE_INFO,
        .next = &binding,
        .createFlags = 0,
        .systemId = system_id
    };

    XrResult result = xrCreateSession(
        xr_instance,
        &session_info,
        &session
    );

    if (!xr_check_result(result, "xrCreateSession")) {
        return 0;
    }

    XrReferenceSpaceCreateInfo space_info = {
        .type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO,
        .next = NULL,
        .referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL,
        .poseInReferenceSpace.orientation.w = 1.0f
    };

    result = xrCreateReferenceSpace(
        session,
        &space_info,
        &space
    );

    return xr_check_result(
        result,
        "xrCreateReferenceSpace"
    );
}

static int xr_setup_swapchains(void) {
    uint32_t format_count = 0;

    XrResult result = xrEnumerateSwapchainFormats(
        session,
        0,
        &format_count,
        NULL
    );

    if (!xr_check_result(
        result,
        "xrEnumerateSwapchainFormats(count)"
    )) {
        return 0;
    }

    if (format_count == 0) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "[XR] No swapchain formats"
        );
        return 0;
    }

    int64_t *formats = malloc(
        sizeof(int64_t) * format_count
    );

    if (!formats) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "[XR] Format allocation failed"
        );
        return 0;
    }

    result = xrEnumerateSwapchainFormats(
        session,
        format_count,
        &format_count,
        formats
    );

    if (!xr_check_result(
        result,
        "xrEnumerateSwapchainFormats"
    )) {
        free(formats);
        return 0;
    }

    int64_t format = formats[0];

    for (uint32_t i = 0; i < format_count; i++) {
        if (formats[i] == GL_RGBA8) {
            format = GL_RGBA8;
            break;
        }
    }

    SDL_Log(
        "[XR] Swapchain format: %lld",
        (long long)format
    );

    free(formats);

    for (int eye = 0; eye < 2; eye++) {
        XrSwapchainCreateInfo info = {
            .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
            .next = NULL,
            .createFlags = 0,
            .usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
            .format = format,
            .sampleCount =
                views_cfg[eye].recommendedSwapchainSampleCount,
            .width =
                views_cfg[eye].recommendedImageRectWidth,
            .height =
                views_cfg[eye].recommendedImageRectHeight,
            .faceCount = 1,
            .arraySize = 1,
            .mipCount = 1
        };

        result = xrCreateSwapchain(
            session,
            &info,
            &swapchain[eye]
        );

        if (!xr_check_result(
            result,
            "xrCreateSwapchain"
        )) {
            return 0;
        }

        result = xrEnumerateSwapchainImages(
            swapchain[eye],
            0,
            &image_count[eye],
            NULL
        );

        if (!xr_check_result(
            result,
            "xrEnumerateSwapchainImages(count)"
        )) {
            return 0;
        }

        images[eye] = calloc(
            image_count[eye],
            sizeof(*images[eye])
        );

        if (!images[eye]) {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "[XR] Swapchain image allocation failed"
            );
            return 0;
        }

        for (uint32_t i = 0; i < image_count[eye]; i++) {
            images[eye][i].type =
                XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR;
        }

        result = xrEnumerateSwapchainImages(
            swapchain[eye],
            image_count[eye],
            &image_count[eye],
            (XrSwapchainImageBaseHeader *)images[eye]
        );

        if (!xr_check_result(
            result,
            "xrEnumerateSwapchainImages"
        )) {
            return 0;
        }
    }

    SDL_Log("[XR] Swapchains ready");

    return 1;
}

static int xr_setup(void) {
    if (!xr_setup_system()) {
        return 0;
    }

    if (!xr_setup_views()) {
        return 0;
    }

    if (!xr_setup_session()) {
        return 0;
    }

    if (!xr_setup_swapchains()) {
        return 0;
    }

    SDL_Log("[XR] Setup complete");
    return 1;
}

static int xr_begin_frame(XrFrameState *frame_state) {
    XrResult result = xrWaitFrame(
        session,
        &(XrFrameWaitInfo){
            .type = XR_TYPE_FRAME_WAIT_INFO
        },
        frame_state
    );

    if (XR_FAILED(result)) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "[XR] xrWaitFrame failed: %d",
            result
        );
        return 0;
    }

    result = xrBeginFrame(
        session,
        &(XrFrameBeginInfo){
            .type = XR_TYPE_FRAME_BEGIN_INFO
        }
    );

    if (XR_FAILED(result)) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "[XR] xrBeginFrame failed: %d",
            result
        );
        return 0;
    }

    return 1;
}

static int xr_locate_views(
    XrFrameState *frame_state,
    XrView views[2]
) {
    XrViewState view_state = {
        .type = XR_TYPE_VIEW_STATE
    };

    XrViewLocateInfo locate_info = {
        .type = XR_TYPE_VIEW_LOCATE_INFO,
        .viewConfigurationType =
            XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
        .displayTime =
            frame_state->predictedDisplayTime,
        .space = space
    };

    uint32_t view_count = 0;

    XrResult result = xrLocateViews(
        session,
        &locate_info,
        &view_state,
        2,
        &view_count,
        views
    );

    if (XR_FAILED(result) || view_count != 2) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "[XR] xrLocateViews failed: %d count=%u",
            result,
            view_count
        );
        return 0;
    }

    return 1;
}

static int xr_render_eye(
    int eye,
    const XrView *view,
    GLuint program,
    GLuint vbo
) {
    uint32_t image_index = 0;

    XrResult result = xrAcquireSwapchainImage(
        swapchain[eye],
        &(XrSwapchainImageAcquireInfo){ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO },
        &image_index
    );

    if (XR_FAILED(result))
        return 0;

    result = xrWaitSwapchainImage(
        swapchain[eye],
        &(XrSwapchainImageWaitInfo){
            XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO,
            NULL,
            XR_INFINITE_DURATION
        }
    );

    if (XR_FAILED(result))
        return 0;

    const int width = views_cfg[eye].recommendedImageRectWidth;
    const int height = views_cfg[eye].recommendedImageRectHeight;

    GLuint fbo, depth;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &depth);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        images[eye][image_index].image,
        0
    );

    render_camera(view, width, height, program, vbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteRenderbuffers(1, &depth);
    glDeleteFramebuffers(1, &fbo);

    xrReleaseSwapchainImage(
        swapchain[eye],
        &(XrSwapchainImageReleaseInfo){ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO }
    );

    return 1;
}


// render all our eyes
static void xr_render(
    GLuint program,
    GLuint vbo
) {
    XrFrameState frame_state = {
        .type = XR_TYPE_FRAME_STATE
    };

    if (!xr_begin_frame(&frame_state)) {
        return;
    }

    /*
     * Runtime can tell us not to render this frame.
     * We still MUST end the frame.
     */
    if (!frame_state.shouldRender) {
        xrEndFrame(
            session,
            &(XrFrameEndInfo){
                .type = XR_TYPE_FRAME_END_INFO,
                .displayTime = frame_state.predictedDisplayTime,
                .environmentBlendMode =
                    XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
                .layerCount = 0,
                .layers = NULL
            }
        );
        return;
    }

    XrView views[2] = {
        { .type = XR_TYPE_VIEW },
        { .type = XR_TYPE_VIEW }
    };

    if (!xr_locate_views(&frame_state, views)) {
        xrEndFrame(
            session,
            &(XrFrameEndInfo){
                .type = XR_TYPE_FRAME_END_INFO,
                .displayTime = frame_state.predictedDisplayTime,
                .environmentBlendMode =
                    XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
                .layerCount = 0,
                .layers = NULL
            }
        );
        return;
    }

    xr_render_eye(
        0,
        &views[0],
        program,
        vbo
    );

    xr_render_eye(
        1,
        &views[1],
        program,
        vbo
    );

    XrCompositionLayerProjectionView projection_views[2] = {
        {
            .type =
                XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW
        },
        {
            .type =
                XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW
        }
    };

    for (int eye = 0; eye < 2; eye++) {
        projection_views[eye].pose = views[eye].pose;
        projection_views[eye].fov = views[eye].fov;

        projection_views[eye].subImage.swapchain =
            swapchain[eye];

        projection_views[eye].subImage.imageRect.offset =
            (XrOffset2Di){0, 0};

        projection_views[eye].subImage.imageRect.extent =
            (XrExtent2Di){
                (int32_t)views_cfg[eye]
                    .recommendedImageRectWidth,
                (int32_t)views_cfg[eye]
                    .recommendedImageRectHeight
            };

        projection_views[eye]
            .subImage
            .imageArrayIndex = 0;
    }

    XrCompositionLayerProjection layer = {
        .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION,
        .next = NULL,
        .layerFlags = 0,
        .space = space,
        .viewCount = 2,
        .views = projection_views
    };

    const XrCompositionLayerBaseHeader *layers[] = {
        (const XrCompositionLayerBaseHeader *)&layer
    };

    XrFrameEndInfo end_info = {
        .type = XR_TYPE_FRAME_END_INFO,
        .next = NULL,
        .displayTime = frame_state.predictedDisplayTime,
        .environmentBlendMode =
            XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
        .layerCount = 1,
        .layers = layers
    };

    XrResult result = xrEndFrame(
        session,
        &end_info
    );

    if (XR_FAILED(result)) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "[XR] xrEndFrame failed: %d",
            result
        );
    }
}


// --------------------------------------------------
// Main
// --------------------------------------------------

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    SDL_Log("[XR] Starting cube test");

    SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "0");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "[XR] SDL_Init failed: %s",
            SDL_GetError()
        );
        return 1;
    }

    if (!egl_setup()) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "[XR] EGL setup failed"
        );
        SDL_Quit();
        return 1;
    }

    if (!zox_xr_init()) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "[XR] OpenXR initialization failed"
        );
        egl_shutdown();
        SDL_Quit();
        return 1;
    }

    if (!xr_setup()) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "[XR] OpenXR setup failed"
        );
        zox_xr_shutdown();
        egl_shutdown();
        SDL_Quit();
        return 1;
    }

    // Iniitalize OpenGL Data

    GLuint program = make_program();
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(cube),
        cube,
        GL_STATIC_DRAW
    );

    // Iniitalize Cube Data
    spawn_cubes();

    SDL_Log("[XR] Cube renderer ready");

    running = 1;

    while (running) {
        XrEventDataBuffer event_buffer = {
            .type = XR_TYPE_EVENT_DATA_BUFFER
        };

        XrResult result = xrPollEvent(
            xr_instance,
            &event_buffer
        );

        if (result == XR_SUCCESS) {
            XrEventDataSessionStateChanged *state =
                (XrEventDataSessionStateChanged *)&event_buffer;

            if (state->type ==
                XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED) {

                SDL_Log(
                    "[XR] Session state: %d",
                    state->state
                );

                if (state->state == XR_SESSION_STATE_READY) {
                    result = xrBeginSession(
                        session,
                        &(XrSessionBeginInfo){
                            .type = XR_TYPE_SESSION_BEGIN_INFO,
                            .primaryViewConfigurationType =
                                XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO
                        }
                    );

                    if (XR_SUCCEEDED(result)) {
                        session_running = 1;
                        SDL_Log("[XR] Session started");
                    } else {
                        SDL_LogError(
                            SDL_LOG_CATEGORY_APPLICATION,
                            "[XR] xrBeginSession failed: %d",
                            result
                        );
                    }
                }

                if (state->state == XR_SESSION_STATE_STOPPING) {
                    xrEndSession(session);
                    session_running = 0;
                    SDL_Log("[XR] Session stopped");
                }

                if (state->state == XR_SESSION_STATE_EXITING ||
                    state->state == XR_SESSION_STATE_LOSS_PENDING) {
                    running = 0;
                }
            }
        } else if (result != XR_EVENT_UNAVAILABLE) {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "[XR] xrPollEvent failed: %d",
                result
            );
        }

        if (session_running) {
            xr_render(program, vbo);
        }
    }

    SDL_Log("[XR] Shutting down");

    if (session_running) {
        xrEndSession(session);
        session_running = 0;
    }

    if (space != XR_NULL_HANDLE) {
        xrDestroySpace(space);
        space = XR_NULL_HANDLE;
    }

    for (int eye = 0; eye < 2; eye++) {
        free(images[eye]);
        images[eye] = NULL;

        if (swapchain[eye] != XR_NULL_HANDLE) {
            xrDestroySwapchain(swapchain[eye]);
            swapchain[eye] = XR_NULL_HANDLE;
        }
    }

    if (session != XR_NULL_HANDLE) {
        xrDestroySession(session);
        session = XR_NULL_HANDLE;
    }

    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);

    zox_xr_shutdown();
    egl_shutdown();
    SDL_Quit();

    return 0;
}