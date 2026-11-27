// res/shaders/view.glsl, added before every shader's source as it loads (doc/howto/shaders.md): the camera's view
//  block and what reads it, so no shader declares them itself
char* shader_view_source = NULL;

void load_shader_view(const char* directory) {
    char* path = concat_file_path(directory, "view.glsl");
    shader_view_source = zox_read_shader(path);
    free(path);
    if (!shader_view_source) {
        zox_loge("[view.glsl] not found, shaders won't compile");
    }
}

void dispose_shader_view() {
    free(shader_view_source);
    shader_view_source = NULL;
}

// source after the view, malloc'd: #line keeps compile errors at the source's own lines
//  - fragment: zox_fragment defined, for the view's fragment only parts (the fog reads gl_FragCoord)
char* shader_add_view(const char* source, byte fragment) {
    const char* view = shader_view_source ? shader_view_source : "";
    const char* stage = fragment ? "#define zox_fragment\n" : "";
    const char* line = "\n#line 1\n";
    char* output = malloc(strlen(stage) + strlen(view) + strlen(line) + strlen(source) + 1);
    if (!output) {
        return NULL;
    }
    strcpy(output, stage);
    strcat(output, view);
    strcat(output, line);
    strcat(output, source);
    return output;
}
