// Function to check if glGenFramebuffers is available
byte is_frame_buffer_supported() {
    return SDL_GL_ExtensionSupported("GL_ARB_framebuffer_object");
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
        zox_logw("Frame Buffer not supported.");
        return 1;
    }
    /*if (!is_frame_buffer_supported()) {
        zox_log(" ! frame buffer is unsupported.\n")
    } else {
        zox_log(" + frame buffer is supported.\n")
    }*/
    zox_log("UBO Size is [%i]", zox_get_safe_ubo_size());
    return 0;
}
