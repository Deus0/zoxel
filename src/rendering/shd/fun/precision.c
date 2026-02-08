char* append_shader_precision(const char* source, byte level) {

    // precision mediump float;\n"
    char append[32];

    snprintf(append, 32, "precision %sp float;\n",  level == 0 ? "low" : level == 1 ? "medium" : "high");

    size_t length = strlen(source) + strlen(append) + 1;

    char* output = malloc(length);
    if (!output) {
        return NULL;
    }

    output[0] = '\0';
    strcat(output, append);
    strcat(output, source);

    return output;
}
