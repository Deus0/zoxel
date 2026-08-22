static void xr_test_render_cubes(float4x4 vp_matrix) {
    const float spacing = 8.0f;
    static GLuint program = 0;
    static GLuint vbo = 0;

    static const float cube[] = {
        -1,-1,-1, 1,0,0,  1,-1,-1, 1,0,0,  1,1,-1, 1,0,0,
        -1,-1,-1, 1,0,0,  1,1,-1, 1,0,0, -1,1,-1, 1,0,0,

        -1,-1, 1, 0,1,0,  1,1, 1, 0,1,0,  1,-1,1, 0,1,0,
        -1,-1, 1, 0,1,0, -1,1,1, 0,1,0,  1,1,1, 0,1,0,

        -1,-1,-1, 0,0,1, -1,1,-1, 0,0,1, -1,1,1, 0,0,1,
        -1,-1,-1, 0,0,1, -1,1,1, 0,0,1, -1,-1,1, 0,0,1,

         1,-1,-1, 1,1,0,  1,-1,1, 1,1,0,  1,1,1, 1,1,0,
         1,-1,-1, 1,1,0,  1,1,1, 1,1,0,  1,1,-1, 1,1,0,

        -1,1,-1, 0,1,1,  1,1,-1, 0,1,1,  1,1,1, 0,1,1,
        -1,1,-1, 0,1,1,  1,1,1, 0,1,1, -1,1,1, 0,1,1,

        -1,-1,-1, 1,0,1, -1,-1,1, 1,0,1,  1,-1,1, 1,0,1,
        -1,-1,-1, 1,0,1,  1,-1,1, 1,0,1,  1,-1,-1, 1,0,1
    };

    if (!program) {
        const char *vertex_source =
            "#version 300 es\n"
            "layout(location = 0) in vec3 position;\n"
            "layout(location = 1) in vec3 color;\n"
            "uniform mat4 vp;\n"
            "out vec3 v_color;\n"
            "void main() {\n"
            "    gl_Position = vp * vec4(position, 1.0);\n"
            "    v_color = color;\n"
            "}\n";

        const char *fragment_source =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec3 v_color;\n"
            "out vec4 frag_color;\n"
            "void main() {\n"
            "    frag_color = vec4(v_color, 1.0);\n"
            "}\n";

        GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertex_source, NULL);
        glCompileShader(vertex);

        GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragment_source, NULL);
        glCompileShader(fragment);

        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(cube),
            cube,
            GL_STATIC_DRAW
        );
    }

    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*) 0
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*) (3 * sizeof(float))
    );

    GLint vp_location = glGetUniformLocation(program, "vp");

    /*
     * Just draw cubes directly around the origin.
     * No prs_to_transform_matrix dependency.
     */

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            for (int z = -2; z <= 2; z++) {
                float4x4 model = float4x4_identity;

                model.w.x = x * spacing;
                model.w.y = y * spacing;
                model.w.z = z * spacing;

                float4x4 mvp = matrix_multiply(vp_matrix, model);

                glUniformMatrix4fv(
                    vp_location,
                    1,
                    GL_FALSE,
                    (const float*) &mvp
                );

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
