byte zox_init_glew() {
#ifdef zox_windows
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        zox_loge("[glewInit] Failure: [%s]", glewGetErrorString(err));
        return EXIT_FAILURE;
    }
#endif
    return EXIT_SUCCESS;
}
