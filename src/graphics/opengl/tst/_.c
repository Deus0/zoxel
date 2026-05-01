#include "shader.c"

byte test_graphics() {

    if (is_log_opengl) {
        print_opengl();
    }

    if (test_opengl_shaders()) {
        zox_log_error("Shaders not supported");
        // zox_use_post_processing = 0;
        return EXIT_FAILURE;
    }

    if (test_frame_buffer()) {
        zox_logw("Post Processing Disabled.");
        // zox_use_post_processing = 0;
    }

    test_compute();

    // check_geometry();

    return EXIT_SUCCESS;
}
