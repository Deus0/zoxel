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
