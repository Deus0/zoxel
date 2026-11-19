// Saves the frame being drawn as a binary PPM, i.e. so tests can look at the rendering; returns 1 on success
static inline byte zox_gpu_save_screenshot(const char* path, int2 size) {
    byte* pixels = zox_gpu_read_screen(size);
    if (!pixels) {
        return 0;
    }
    FILE* file = fopen(path, "wb");
    if (!file) {
        free(pixels);
        return 0;
    }
    fprintf(file, "P6\n%i %i\n255\n", size.x, size.y);
    fwrite(pixels, 1, (size_t) size.x * size.y * 3, file);
    fclose(file);
    free(pixels);
    return 1;
}
