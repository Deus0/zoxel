byte test_opengl_shaders() {

    uint shader_vertex = glCreateShader(GL_VERTEX_SHADER);
    if (!shader_vertex) {
        zox_log_error("Cannot create shader (vertex) [%i]", GL_VERTEX_SHADER);
        return 1;
    }
    glDeleteShader(shader_vertex);

    uint shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    if (!shader_fragment) {
        zox_log_error("Cannot create shader (fragment) [%i]", GL_FRAGMENT_SHADER);
        return 1;
    }
    glDeleteShader(shader_fragment);

    return 0;   // no errors
}
