byte zox_init_glew() {
#ifdef zox_windows
    zox_logv("Initializing Glew");
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        zox_loge("[initialize_rendering] failed at [zox_init_glew]");
        zox_loge("[glewInit] Failure: [%s]", glewGetErrorString(err));
        return EXIT_FAILURE;
    }
#endif
    return EXIT_SUCCESS;
}
