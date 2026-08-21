// NOTE: String functions!

char* zox_copy_string(const char* src) {
    if (!src) {
        return NULL;
    }
    size_t len = strlen(src) + 1;
    char* out = malloc(len);
    if (out) {
        memcpy(out, src, len);
    }
    return out;
}

char* clone_str(const char* text) {
    int length = strlen(text);
    char *new_text = malloc(length);
    memcpy(new_text, text, length);
    return new_text;
}