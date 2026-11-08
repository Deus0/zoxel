// Prints every uniform/attribute the linked program actually exposes
void shader_log_active_inputs(GLuint program, const char *shader_name) {
    GLint count = 0;
    GLint size;
    GLenum type;
    char name[128];
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
    zox_log("(%s) active uniforms [%i]", shader_name, count);
    for (GLint i = 0; i < count; i++) {
        glGetActiveUniform(program, (GLuint) i, sizeof(name), NULL, &size, &type, name);
        zox_log("   - uniform [%s] at [%i]", name, glGetUniformLocation(program, name));
    }
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);
    zox_log("(%s) active attributes [%i]", shader_name, count);
    for (GLint i = 0; i < count; i++) {
        glGetActiveAttrib(program, (GLuint) i, sizeof(name), NULL, &size, &type, name);
        zox_log("   - attribute [%s] at [%i]", name, glGetAttribLocation(program, name));
    }
}

GLint shader_uniform(GLuint program, const char *name, const char *shader_name) {
    const GLint location = glGetUniformLocation(program, name);
    if (location == -1) {
        zox_loge("(%s) uniform [%s] not found (renamed, misspelled, or unused and stripped)", shader_name, name);
        shader_log_active_inputs(program, shader_name);
    }
    return location;
}

GLint shader_attribute(GLuint program, const char *name, const char *shader_name) {
    const GLint location = glGetAttribLocation(program, name);
    if (location == -1) {
        zox_loge("(%s) attribute [%s] not found (renamed, misspelled, or unused and stripped)", shader_name, name);
        shader_log_active_inputs(program, shader_name);
    }
    return location;
}
