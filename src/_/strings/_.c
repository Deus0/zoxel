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
