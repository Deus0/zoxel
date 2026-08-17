#include "shader.c"
// TODO: Use gpu setting
byte zox_fbos_supported = 1;
extern byte zox_disable_post_processing;

byte test_graphics() {
    if (is_log_opengl) {
        print_opengl();
    }
    if (test_opengl_shaders()) {
        zox_log_error("Shaders not supported");
        return EXIT_FAILURE;
    }
    if (test_frame_buffer()) {
        zox_logw("Post Processing Disabled.");
        zox_disable_post_processing = 1;
        zox_fbos_supported = 0;
    }
    test_compute();
    // check_geometry();
    return EXIT_SUCCESS;
}
