void clear_regular_buffer(uint *gpu_buffer) {
    if (gpu_buffer) {
        glDeleteBuffers(1, gpu_buffer);
        *gpu_buffer = 0;
    }
}

byte initialize_material(uint material, uint vert_shader, uint frag_shader) {
    if (!vert_shader || !frag_shader) {
        zox_log_error("[initialize_materialial]: invalid shader");
        return EXIT_FAILURE;
    }
    glAttachShader(material, vert_shader);
    glAttachShader(material, frag_shader);
    glLinkProgram(material);
    GLint success;
    glGetProgramiv(material, GL_LINK_STATUS, &success);
    // if errors
    if (success != GL_TRUE) {
        GLint info_log_length;
        glGetProgramiv(material, GL_INFO_LOG_LENGTH, &info_log_length);
        GLchar* info_log = malloc(info_log_length);
        glGetProgramInfoLog(material, info_log_length, NULL, info_log);
        zox_log_error("[initialize_material]: material [%i] Shader [%ix%i]\n [%s]", (int) material, vert_shader, frag_shader, info_log);
        free(info_log);
        glDetachShader(material, vert_shader);
        glDetachShader(material, frag_shader);
        return EXIT_FAILURE;
    }
    glDetachShader(material, vert_shader);
    glDetachShader(material, frag_shader);
    return EXIT_SUCCESS;
}

uint spawn_gpu_material_program(const uint2 shader) {
    uint material = glCreateProgram();
    if (initialize_material(material, shader.x, shader.y) ==  EXIT_FAILURE) {
        glDeleteProgram(material);
        return 0;
    } else {
#ifdef zoxel_catch_opengl_errors
        check_opengl_error("spawn_gpu_material_program");
#endif
        return material;
    }
}

byte check_shader_compile_status(uint shader) {
    /*GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint info_log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
        GLchar* info_log = malloc(info_log_length);
        glGetShaderInfoLog(shader, info_log_length, NULL, info_log);
        zox_log_error("###\n[glCompileShader] failed:\n[%s]\n###\n", info_log);
        free(info_log);
        return 1;
    }*/

    int length = 0;
    glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &length);

    if (!length) {
        return 0;
    }

    char* log = malloc(length);
    if (!log) {
        return 1;
    }

    glGetProgramInfoLog(shader, length, NULL, log);
    fprintf(stderr, "Shader Error: %s\n", log);
    free(log);

    return 1;
}

byte compile_shader(
    GLenum shader_type,
    uint* output,
    const GLchar* buffer
) {
    uint shader = glCreateShader(shader_type);
    if (!shader) {
        zox_log_error("Shader not created of type [%i]", shader_type);
        return 1;
    }

    glShaderSource(shader, 1, (const GLchar **) &buffer, NULL);
    glCompileShader(shader);
    // glValidateProgram(shader);
    if (check_shader_compile_status(shader)) {
        return 1;
    }

    *output = shader;
    return 0;
}

uint2 zox_gpu_compile_shader(
    const GLchar* vert_buffer,
    const GLchar* frag_buffer
) {
    if (render_backend != zox_render_backend_opengl) {
        zox_logw("Render Backend not OpenGL");
        return uint2_zero;
    }
    uint2 shader = uint2_zero;
    if (compile_shader(GL_VERTEX_SHADER, &shader.x, vert_buffer)) {
        zox_log_error("[compile_shader] vert\n\n[%s]\n", vert_buffer);
        return (uint2) { 0, 0 };
    }
    if (compile_shader(GL_FRAGMENT_SHADER, &shader.y, frag_buffer)) {
        zox_log_error("[compile_shader] frag\n\n[%s]\n", frag_buffer);
        return (uint2) { 0, 0 };
    }
    zox_log("Returning shader from compiler: %ix%i", shader.x, shader.y);
    return shader;
}
