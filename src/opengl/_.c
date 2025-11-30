// OpenGL Wrapper for Zoxel
#ifndef zoxm_opengl
#define zoxm_opengl

byte zox_use_post_processing = 1;
#include "dat/_.c"
#include "fun/_.c"

byte zox_init_glew() {
#ifdef zox_windows
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        zox_log_errno("[glewInit] failed - [%s]", glewGetErrorString(err));
        return EXIT_FAILURE;
    }
#endif
    return EXIT_SUCCESS;
}

void initialize_opengl() {
    check_frame_buffer();
    test_compute();

    zox_log("GL_VERTEX_SHADER: %d", GL_VERTEX_SHADER);

    // check_geometry();
    if (is_log_sdl) {
        zox_log_sdl("zoxel opengl version [%u]", shader_opengl_version)
        print_opengl();
    }
    zox_logi("UBO Size [%i]", zox_get_safe_ubo_size());
}

#endif
