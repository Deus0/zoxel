byte save_texture_to_bmp(const char *path, const TextureData *data, int2 size) {
    FILE *f = fopen(path, "wb");
    if (!f) {
        zox_loge("Failed to open BMP output: %s", path);
        return 0;
    }
    int width = size.x;
    int height = size.y;
    int row_size = width * 4;
    uint32_t image_size = row_size * height;
    BMPFileHeader file = {
        .bfType = 0x4D42,
        .bfSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + image_size,
        .bfReserved1 = 0,
        .bfReserved2 = 0,
        .bfOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader)
    };
    BMPInfoHeader info = {
        .biSize = sizeof(BMPInfoHeader),
        .biWidth = width,
        .biHeight = height, // top-down BMP
        .biPlanes = 1,
        .biBitCount = 32,
        .biCompression = 0,
        .biSizeImage = image_size,
        .biXPelsPerMeter = 0,
        .biYPelsPerMeter = 0,
        .biClrUsed = 0,
        .biClrImportant = 0
    };
    fwrite(&file, sizeof(file), 1, f);
    fwrite(&info, sizeof(info), 1, f);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            color *src = &data->value[y * width + x];

            byte pixel[4];
            pixel[0] = ((byte*)src)[2]; // B
            pixel[1] = ((byte*)src)[1]; // G
            pixel[2] = ((byte*)src)[0]; // R
            pixel[3] = ((byte*)src)[3]; // A

            fwrite(pixel, 4, 1, f);
        }
    }
    fclose(f);
    zox_log("Saved BMP [%s] %ix%i", path, width, height);
    return 1;
}
