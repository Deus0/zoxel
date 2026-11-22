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
    // a png beside it, to look at (an image viewer opens it, a ppm few do)
    const size_t length = strlen(path);
    if (length > 4 && !strcmp(path + length - 4, ".ppm")) {
        char png[1024];
        snprintf(png, sizeof(png), "%.*s.png", (int) (length - 4), path);
        zox_save_png(png, pixels, size.x, size.y);
    }
    free(pixels);
    return 1;
}
