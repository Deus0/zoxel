// The frame being drawn (the back buffer) as rgb pixels, top row first, free it after; NULL if it can't
//  - call before the buffer swap, after everything is drawn
//  - size: the window's, the whole screen whichever viewport was drawn last (split screen, viewport cameras)
//  - read by screenshots (below) and the gif recorder (graphics/fun/gif.c)
static inline byte* zox_gpu_read_screen(int2 size) {
    if (size.x <= 0 || size.y <= 0) {
        return NULL;
    }
    byte* rgba = malloc((size_t) size.x * size.y * 4);
    byte* rgb = malloc((size_t) size.x * size.y * 3);
    if (!rgba || !rgb) {
        free(rgba);
        free(rgb);
        return NULL;
    }
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    // gl rows start at the bottom
    for (int y = 0; y < size.y; y++) {
        const byte* from = &rgba[(size_t) (size.y - 1 - y) * size.x * 4];
        byte* to = &rgb[(size_t) y * size.x * 3];
        for (int x = 0; x < size.x; x++) {
            to[x * 3] = from[x * 4];
            to[x * 3 + 1] = from[x * 4 + 1];
            to[x * 3 + 2] = from[x * 4 + 2];
        }
    }
    free(rgba);
    return rgb;
}

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
