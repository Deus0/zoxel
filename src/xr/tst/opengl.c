// --------------------------------------------------
// OpenGL
//
//  -Copies of Zoxel functions, kept for test until derived from opengl/_.c
//
// --------------------------------------------------

static GLuint make_shader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    return s;
}

static GLuint make_program(void) {
    const char *vs =
        "#version 300 es\n"
        "layout(location=0) in vec3 p;\n"
        "layout(location=1) in vec3 c;\n"
        "uniform mat4 vp;\n"
        "out vec3 col;\n"
        "void main(){gl_Position=vp*vec4(p,1.0);col=c;}";
    const char *fs =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec3 col;\n"
        "out vec4 o;\n"
        "void main(){o=vec4(col,1.0);}";
    GLuint p = glCreateProgram();
    GLuint v = make_shader(GL_VERTEX_SHADER, vs);
    GLuint f = make_shader(GL_FRAGMENT_SHADER, fs);
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}


void initialize_camera_buffers(GLuint* fbo, GLuint* rbo) {
    glGenFramebuffers(1, fbo);
    glGenRenderbuffers(1, rbo);
}

void dispose_camera_buffers(GLuint* fbo, GLuint* rbo) {
    glDeleteRenderbuffers(1, rbo);
    glDeleteFramebuffers(1, fbo);
}