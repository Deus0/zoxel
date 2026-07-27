byte bmp_transparency_hack = 1;

#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BMPFileHeader;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BMPInfoHeader;
#pragma pack(pop)

byte load_texture_from_bmp(const char *path, TextureData *data, int2 *size) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        zox_loge("Error: fopen failed: %s", path);
        return 0;
    }
    BMPFileHeader file;
    if (fread(&file, sizeof(file), 1, f) != 1) {
        zox_loge("Error: failed to read BMP file header: %s", path);
        fclose(f);
        return 0;
    }

    if (file.bfType != 0x4D42) { // 'BM'
        zox_loge("Error: not a BMP file: %s", path);
        fclose(f);
        return 0;
    }
    BMPInfoHeader info;
    if (fread(&info, sizeof(info), 1, f) != 1) {
        zox_loge("Error: failed to read BMP info header: %s", path);
        fclose(f);
        return 0;
    }
    if (info.biCompression != 0) {
        zox_loge("Error: compressed BMP not supported: %s", path);
        fclose(f);
        return 0;
    }
    if (info.biBitCount != 24 && info.biBitCount != 32) {
        zox_loge("Error: only 24/32-bit BMP supported: %s", path);
        fclose(f);
        return 0;
    }
    int width = info.biWidth;
    int height = info.biHeight;
    byte flip = 0;
    /*int flip = 1;
    if (height < 0) {
        height = -height;
        flip = 0; // already top-down
    }*/
    size->x = width;
    size->y = height;
    resize_TextureData(data, (uint)(width * height));
    if (fseek(f, file.bfOffBits, SEEK_SET) != 0) {
        zox_loge("Error: failed to seek to pixel data: %s", path);
        fclose(f);
        return 0;
    }
    int bytes_per_pixel = info.biBitCount / 8;
    int row_padded = ((width * bytes_per_pixel + 3) / 4) * 4;
    byte *row = (byte*) malloc(row_padded);
    if (!row) {
        zox_loge("Error: malloc failed");
        fclose(f);
        return 0;
    }
    // zox_log("+ Loading BMP: %s - flip [%i], bytes_per_pixel [%i]", path, flip, bytes_per_pixel);
    for (int y = 0; y < height; y++) {
        if (fread(row, 1, row_padded, f) != (size_t)row_padded) {
            zox_loge("Error: failed to read BMP row");
            free(row);
            fclose(f);
            return 0;
        }

        int dst_y = flip ? (height - 1 - y) : y;
        color *dst = data->value + dst_y * width;

        for (int x = 0; x < width; x++) {
            byte b = row[x * bytes_per_pixel + 0];
            byte g = row[x * bytes_per_pixel + 1];
            byte r = row[x * bytes_per_pixel + 2];
            byte a = (bytes_per_pixel == 4) ? row[x * bytes_per_pixel + 3] : 255;

            // zox_log("RGB [%ix%ix%i]", r, g, b);
            if (bmp_transparency_hack && bytes_per_pixel == 3 && r == 0 && g == 0 && b == 0) {
                a = 0;
            }

            // assuming color = 0xAARRGGBB or similar 4-byte layout
            ((byte*)&dst[x])[0] = r;
            ((byte*)&dst[x])[1] = g;
            ((byte*)&dst[x])[2] = b;
            ((byte*)&dst[x])[3] = a;
        }
    }
    free(row);
    fclose(f);
    return 1;
}
