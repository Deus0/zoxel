// uses opengl version to check iif compute is supported
int check_compute_shader_support_from_version() {
#ifndef zox_disable_compute
    const char* version_str = (const char*) glGetString(GL_VERSION);
    if (!version_str) {
        // zox_logw("GL not enabled.");
        return EXIT_FAILURE;
    }
    int is_opengl_es = strstr(version_str, "ES") != NULL;
    int major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    if (is_opengl_es) {
        if (major >= 3 && minor >= 1) {
            zox_log_opengl("+ compute supported: OpenGL ES %d.%d [%s]", major, minor, version_str)
            return EXIT_SUCCESS;
        }
    } else {
        if (major >= 4 && minor >= 3) {
            zox_log_opengl("+ compute supported: OpenGL %d.%d [%s]", major, minor, version_str)
            return EXIT_SUCCESS;
        }
    }
    if (is_log_opengl) {
        zox_loge("compute not supported: OpenGL %d.%d [%s]", major, minor, version_str)
    }
#endif
    return EXIT_FAILURE;
}

// debugs compute shader support
int check_compute_shader_support() {
    check_opengl_error("[pre check_compute_shader_support Error]");
    if (check_compute_shader_support_from_version() == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
#ifdef zoxel_debug_opengl
    zox_log("    > opengl compute stats\n")
#endif
    return EXIT_SUCCESS;
}

// removed compute test for now, broke on android version
void test_compute() {
    if (check_compute_shader_support() == EXIT_FAILURE) {
        zox_loge("opengl compute is not supported")
    } else {
        zox_log_opengl("+ opengl compute supported")
    }
}

int test_compute_shader() {
#ifndef zox_disable_compute
    uint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    if (compute_shader == 0) {
        zox_log("    - compute shader creation failed");
        return EXIT_FAILURE;
    } else {
        zox_log("    > compute shader creation success");
        zox_gpu_dispose_shader(compute_shader);
        return EXIT_SUCCESS;
    }
#else
    zox_log("Compute Shaders Disabled");
    return EXIT_FAILURE;
#endif
}

// # Compute: storage buffers, programs and dispatches (gpu meshing, doc/tec/gpu_meshing.md)
//  - needs OpenGL ES 3.1+ or OpenGL 4.3+ at runtime (the game asks for 3.0, drivers give their newest, i.e. 3.2)
//  - storage buffers can also be bound as vertex and index buffers, so a compute pass can write a mesh to draw

// whether the current context runs compute shaders, 0 without a context
byte zox_gpu_compute_supported() {
#ifndef zox_disable_compute
    const char* version = (const char*) glGetString(GL_VERSION);
    if (!version) {
        return 0;
    }
    const byte es = strstr(version, "ES") != NULL;
    int major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    return es ? (major > 3 || (major == 3 && minor >= 1)) : (major > 4 || (major == 4 && minor >= 3));
#else
    return 0;
#endif
}

// compiles and links a compute program from source without a #version line (it's added for the context)
//  - returns the program, 0 on failure (the compile or link log is logged)
guint zox_gpu_compute_program(const char* source) {
#ifndef zox_disable_compute
    const char* version = (const char*) glGetString(GL_VERSION);
    const byte es = version && strstr(version, "ES") != NULL;
    const char* header = es ? "#version 310 es\nprecision highp float;\nprecision highp int;\n" : "#version 430\n";
    const char* sources[2] = { header, source };
    guint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 2, sources, NULL);
    glCompileShader(shader);
    gint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        char log[2048];
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        zox_loge("[compute] shader failed to compile:\n%s", log);
        glDeleteShader(shader);
        return 0;
    }
    guint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);
    glDetachShader(program, shader);
    glDeleteShader(shader);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        char log[2048];
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        zox_loge("[compute] program failed to link:\n%s", log);
        glDeleteProgram(program);
        return 0;
    }
    return program;
#else
    (void) source;
    return 0;
#endif
}

static inline void zox_gpu_dispose_program(guint program) {
    glDeleteProgram(program);
}

// a storage buffer of size bytes, filled from data (NULL leaves it undefined)
guint zox_gpu_storage_buffer(size_t size, const void* data) {
#ifndef zox_disable_compute
    guint buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    return buffer;
#else
    (void) size; (void) data;
    return 0;
#endif
}

// replaces a storage buffer's contents, growing it to size
static inline void zox_gpu_storage_upload(guint buffer, size_t size, const void* data) {
#ifndef zox_disable_compute
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif
}

// writes part of a storage buffer (offset and size in bytes)
static inline void zox_gpu_storage_write(guint buffer, size_t offset, size_t size, const void* data) {
#ifndef zox_disable_compute
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif
}

// copies part of a storage buffer back to the cpu (it waits for the gpu: tests and small counters only)
static inline byte zox_gpu_storage_read(guint buffer, size_t offset, size_t size, void* out) {
#ifndef zox_disable_compute
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    const void* mapped = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, offset, size, GL_MAP_READ_BIT);
    if (mapped) {
        memcpy(out, mapped, size);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    return mapped != NULL;
#else
    (void) buffer; (void) offset; (void) size; (void) out;
    return 0;
#endif
}

static inline void zox_gpu_storage_bind(guint binding, guint buffer) {
#ifndef zox_disable_compute
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);
#endif
}

static inline void zox_gpu_compute_use(guint program) {
    glUseProgram(program);
}

static inline void zox_gpu_compute_uniform_int(guint program, const char* name, int value) {
    glUniform1i(glGetUniformLocation(program, name), value);
}

static inline void zox_gpu_compute_uniform_uint(guint program, const char* name, uint value) {
    glUniform1ui(glGetUniformLocation(program, name), value);
}

// runs the bound program over groups, then makes its writes visible to reads, draws and copies after it
static inline void zox_gpu_compute_dispatch(uint groups_x, uint groups_y, uint groups_z) {
#ifndef zox_disable_compute
    glDispatchCompute(groups_x, groups_y, groups_z);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT |
        GL_ELEMENT_ARRAY_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);
#endif
}

// waits until the gpu finished everything sent so far (timing tests)
static inline void zox_gpu_finish() {
    glFinish();
}

// # Drawing what a compute pass wrote: storage buffers as vertex and index buffers, an indirect draw command
//  - no copy back to the cpu, the gpu reads its own index count (glDrawElementsIndirect, ES 3.1+ / GL 4.3+)

// a draw program from vertex and fragment source without #version lines (added for the context)
guint zox_gpu_draw_program(const char* vertex_source, const char* fragment_source) {
#ifndef zox_disable_compute
    const char* version = (const char*) glGetString(GL_VERSION);
    const byte es = version && strstr(version, "ES") != NULL;
    const char* header = es ? "#version 310 es\nprecision highp float;\nprecision highp int;\n" : "#version 430\n";
    const char* stages_sources[2] = { vertex_source, fragment_source };
    const GLenum stages[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
    guint shaders[2] = { 0, 0 };
    for (byte i = 0; i < 2; i++) {
        const char* sources[2] = { header, stages_sources[i] };
        shaders[i] = glCreateShader(stages[i]);
        glShaderSource(shaders[i], 2, sources, NULL);
        glCompileShader(shaders[i]);
        gint status = GL_FALSE;
        glGetShaderiv(shaders[i], GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            char log[2048];
            glGetShaderInfoLog(shaders[i], sizeof(log), NULL, log);
            zox_loge("[draw program] %s shader failed to compile:\n%s", i ? "fragment" : "vertex", log);
            glDeleteShader(shaders[0]);
            glDeleteShader(shaders[1]);
            return 0;
        }
    }
    guint program = glCreateProgram();
    glAttachShader(program, shaders[0]);
    glAttachShader(program, shaders[1]);
    glLinkProgram(program);
    glDetachShader(program, shaders[0]);
    glDetachShader(program, shaders[1]);
    glDeleteShader(shaders[0]);
    glDeleteShader(shaders[1]);
    gint status = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        char log[2048];
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        zox_loge("[draw program] failed to link:\n%s", log);
        glDeleteProgram(program);
        return 0;
    }
    return program;
#else
    (void) vertex_source; (void) fragment_source;
    return 0;
#endif
}

// a vertex array drawing vec4 vertices (attribute location) with uint indices, both storage buffers
guint zox_gpu_storage_mesh_vao(guint vertices, guint indices, guint location) {
#ifndef zox_disable_compute
    guint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vertices);
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return vao;
#else
    (void) vertices; (void) indices; (void) location;
    return 0;
#endif
}

static inline void zox_gpu_dispose_vao(guint vao) {
#ifndef zox_disable_compute
    glDeleteVertexArrays(1, &vao);
#endif
}

// draws triangles from a vertex array, the index count read by the gpu from command (a DrawElementsIndirectCommand)
static inline void zox_gpu_draw_indirect(guint vao, guint command) {
#ifndef zox_disable_compute
    glBindVertexArray(vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, command);
    glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    glBindVertexArray(0);
#endif
}

static inline void zox_gpu_program_float4x4(guint program, const char* name, float4x4 value) {
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, (const float*) &value);
}

static inline void zox_gpu_program_float4(guint program, const char* name, float4 value) {
    glUniform4f(glGetUniformLocation(program, name), value.x, value.y, value.z, value.w);
}

static inline void zox_gpu_program_float4_array(guint program, const char* name, const float4* values, int count) {
    glUniform4fv(glGetUniformLocation(program, name), count, (const float*) values);
}
