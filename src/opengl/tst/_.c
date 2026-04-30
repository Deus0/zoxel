#include "shader.c"


byte test_opengl() {

    if (is_log_sdl) {
        // zox_log_sdl("test_opengl: OpenGL version [%u]", shader_opengl_version);
        print_opengl();
    }

    if (test_opengl_shaders()) {
        zox_log_error("Shaders not supported");
        // zox_use_post_processing = 0;
        return 1;
    }

    if (test_frame_buffer()) {
        zox_logw("Post Processing Disabled.");
        // zox_use_post_processing = 0;
    }

    test_compute();

    // check_geometry();

    return 0;
}
