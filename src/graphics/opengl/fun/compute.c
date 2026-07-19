// uses opengl version to check iif compute is supported
int check_compute_shader_support_from_version() {
#ifndef zox_disable_compute
    const char* version_str = (const char*) glGetString(GL_VERSION);
    if (!version_str) {
        // zox_logw("GL not enabled.");
        return EXIT_FAILURE;
    }
    int is_opengl_es = strstr(version_str, "ES") != NULL;
    int major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    if (is_opengl_es) {
        if (major >= 3 && minor >= 1) {
            zox_log_opengl("+ compute supported: OpenGL ES %d.%d [%s]", major, minor, version_str)
            return EXIT_SUCCESS;
        }
    } else {
        if (major >= 4 && minor >= 3) {
            zox_log_opengl("+ compute supported: OpenGL %d.%d [%s]", major, minor, version_str)
            return EXIT_SUCCESS;
        }
    }
    if (is_log_opengl) {
        zox_log_error("compute not supported: OpenGL %d.%d [%s]", major, minor, version_str)
    }
#endif
    return EXIT_FAILURE;
}

// debugs compute shader support
int check_compute_shader_support() {
    check_opengl_error("[pre check_compute_shader_support Error]");
    if (check_compute_shader_support_from_version() == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
#ifdef zoxel_debug_opengl
    zox_log("    > opengl compute stats\n")
#endif
    return EXIT_SUCCESS;
}

// removed compute test for now, broke on android version
void test_compute() {
    if (check_compute_shader_support() == EXIT_FAILURE) {
        zox_log_error("opengl compute is not supported")
    } else {
        zox_log_opengl("+ opengl compute supported")
    }
}

int test_compute_shader() {
#ifndef zox_disable_compute
    uint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    if (compute_shader == 0) {
        zox_log("    - compute shader creation failed");
        return EXIT_FAILURE;
    } else {
        zox_log("    > compute shader creation success");
        zox_gpu_dispose_shader(compute_shader);
        return EXIT_SUCCESS;
    }
#else
    zox_log("Compute Shaders Disabled");
    return EXIT_FAILURE;
#endif
}
