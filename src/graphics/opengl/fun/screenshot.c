// Saves the frame being drawn (the back buffer) as a binary PPM, i.e. so tests can look at the rendering
//  - call before the buffer swap, after everything is drawn
//  - returns 1 on success
static inline byte zox_gpu_save_screenshot(const char* path) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const int width = viewport[2];
    const int height = viewport[3];
    if (width <= 0 || height <= 0) {
        return 0;
    }
    unsigned char* pixels = malloc((size_t) width * height * 4);
    if (!pixels) {
        return 0;
    }
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(viewport[0], viewport[1], width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    FILE* file = fopen(path, "wb");
    if (!file) {
        free(pixels);
        return 0;
    }
    fprintf(file, "P6\n%i %i\n255\n", width, height);
    // gl rows start at the bottom
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            fwrite(&pixels[(y * width + x) * 4], 1, 3, file);
        }
    }
    fclose(file);
    free(pixels);
    return 1;
}
