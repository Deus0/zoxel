uint create_shader_compute() {
    return glCreateShader(GL_COMPUTE_SHADER);
}

uint create_shader_vertex() {
    return glCreateShader(GL_VERTEX_SHADER);
}

uint create_shader_fragment() {
    return glCreateShader(GL_FRAGMENT_SHADER);
}

byte initialize_material(uint material, uint vert_shader, uint frag_shader) {

    if (!vert_shader || !frag_shader) {
        zox_log_error("[initialize_materialial]: invalid shader");
        return EXIT_FAILURE;
    }

    gint attached_shaders = 0;

    glAttachShader(material, vert_shader);
    glGetProgramiv(material, GL_ATTACHED_SHADERS, &attached_shaders);
    if (attached_shaders != 1) {
        zox_log_error("Vert Shader failed to attach.");
        return EXIT_FAILURE;
    }

    glAttachShader(material, frag_shader);
    glGetProgramiv(material, GL_ATTACHED_SHADERS, &attached_shaders);
    if (attached_shaders != 2) {
        zox_log_error("Frag Shader failed to attach.");
        return EXIT_FAILURE;
    }

    /*glValidateProgram(material);
    gint validate_status = GL_FALSE;
    glGetProgramiv(material, GL_VALIDATE_STATUS, &validate_status);
    if (validate_status == GL_FALSE) {
        gint info_log_length;
        glGetProgramiv(material, GL_INFO_LOG_LENGTH, &info_log_length);
        if (info_log_length > 0) {
            GLchar* log = malloc(info_log_length + 1);
            glGetProgramInfoLog(material, info_log_length, NULL, log);
            zox_log_error("Material validation failed:\n%s", log);
            free(log);
        } else {
            zox_log_error("Material Validation failed, no log.");
        }

        return EXIT_FAILURE;  // Return early if validation fails
    }*/

    glLinkProgram(material);

    /*GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        zox_log_error("OpenGL error on glLinkProgram(): %i", error);
        return EXIT_FAILURE;
    }*/

    gint success = GL_FALSE;
    glGetProgramiv(material, GL_LINK_STATUS, &success);

    byte output = EXIT_SUCCESS;

    if (success != GL_TRUE) {
        gint info_log_length;
        glGetProgramiv(material, GL_INFO_LOG_LENGTH, &info_log_length);

        if (info_log_length > 0) {
            gchar* log = malloc(info_log_length + 1);
            glGetProgramInfoLog(material, info_log_length, NULL, log);

            zox_log_error("Material Link Failure [%u] to [%u x %u]\n%s", material, vert_shader, frag_shader, log);
            free(log);
        } else {
            zox_log_error("Material Link Failed [%u] to [%u x %u]", material, vert_shader, frag_shader);
        }

        output = EXIT_FAILURE;
    }

    glDetachShader(material, vert_shader);
    glDetachShader(material, frag_shader);

    return output;
}

uint spawn_gpu_material_program(uint2 shader) {

    uint material = glCreateProgram();

    if (initialize_material(material, shader.x, shader.y) == EXIT_FAILURE) {
        glDeleteProgram(material);
        return 0;
    }

    return material;
}

byte check_shader_compile_status(uint shader) {

    /*GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        zox_log_error("OpenGL error on shader compile status: %i", error);
        return 1;
    }*/

    gint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE) {
        gint info_log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
        gchar* info_log = malloc(info_log_length + 1); // +1 for null-terminator

        if (info_log) {
            glGetShaderInfoLog(shader, info_log_length, NULL, info_log);
            zox_log_error("###\n[glCompileShader] failed:\n[%s]\n###\n", info_log);
            free(info_log);
        }

        return 1;
    }

    return 0;
}

byte compile_shader(GLenum shader_type, uint* output, const gchar* buffer) {

    uint shader = glCreateShader(shader_type);
    if (!shader) {
        zox_log_error("Shader not created of type [%i]", shader_type);
        return 1;
    }

    glShaderSource(shader, 1, (const gchar **) &buffer, NULL);
    glCompileShader(shader);
    if (check_shader_compile_status(shader)) {
        return 1;
    }

    *output = shader;
    return 0;
}

uint2 zox_gpu_compile_shader(const gchar* vert_buffer, const gchar* frag_buffer) {

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

    // zox_log("Compiled shader: %ix%i", shader.x, shader.y);

    return shader;
}
