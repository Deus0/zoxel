// Writes rgb pixels (top row first) as a png, uncompressed (deflate's stored blocks): any image viewer opens it,
//  no library needed. Returns 1 on success (a screenshot's copy, graphics/fun/screenshot.c)

static inline uint zox_png_crc(uint crc, const byte* data, size_t length) {
    static uint table[256];
    static byte made = 0;
    if (!made) {
        for (uint n = 0; n < 256; n++) {
            uint c = n;
            for (int k = 0; k < 8; k++) {
                c = c & 1 ? 0xedb88320u ^ (c >> 1) : c >> 1;
            }
            table[n] = c;
        }
        made = 1;
    }
    crc = ~crc;
    for (size_t i = 0; i < length; i++) {
        crc = table[(crc ^ data[i]) & 0xff] ^ (crc >> 8);
    }
    return ~crc;
}

static inline void zox_png_u32(byte* out, uint value) {
    out[0] = (byte) (value >> 24);
    out[1] = (byte) (value >> 16);
    out[2] = (byte) (value >> 8);
    out[3] = (byte) value;
}

static inline void zox_png_chunk(FILE* file, const char* type, const byte* data, uint length) {
    byte head[8];
    zox_png_u32(head, length);
    memcpy(head + 4, type, 4);
    fwrite(head, 1, 8, file);
    if (length) {
        fwrite(data, 1, length, file);
    }
    uint crc = zox_png_crc(0, (const byte*) type, 4);
    crc = zox_png_crc(crc, data, length);
    byte tail[4];
    zox_png_u32(tail, crc);
    fwrite(tail, 1, 4, file);
}

byte zox_save_png(const char* path, const byte* rgb, int width, int height) {
    if (!rgb || width <= 0 || height <= 0) {
        return 0;
    }
    // each row its filter byte (0, none) then its pixels
    const size_t row = (size_t) width * 3 + 1;
    const size_t raw_length = row * height;
    // stored deflate blocks of at most 65535 bytes, a 5 byte header each, in a zlib stream (2 byte header, adler32)
    const size_t blocks = (raw_length + 65534) / 65535;
    const size_t z_length = 2 + raw_length + blocks * 5 + 4;
    byte* z = malloc(z_length);
    if (!z) {
        return 0;
    }
    size_t at = 0;
    z[at++] = 0x78;
    z[at++] = 0x01;
    uint a = 1, b = 0;
    size_t left = raw_length;
    size_t raw_at = 0;
    while (left) {
        const uint length = left > 65535 ? 65535 : (uint) left;
        left -= length;
        z[at++] = left ? 0 : 1;
        z[at++] = (byte) length;
        z[at++] = (byte) (length >> 8);
        z[at++] = (byte) ~length;
        z[at++] = (byte) (~length >> 8);
        for (uint i = 0; i < length; i++, raw_at++) {
            const size_t y = raw_at / row, x = raw_at % row;
            const byte value = x == 0 ? 0 : rgb[y * (size_t) width * 3 + x - 1];
            z[at++] = value;
            a = (a + value) % 65521;
            b = (b + a) % 65521;
        }
    }
    zox_png_u32(z + at, (b << 16) | a);
    at += 4;
    FILE* file = fopen(path, "wb");
    if (!file) {
        free(z);
        return 0;
    }
    const byte signature[8] = { 0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n' };
    fwrite(signature, 1, 8, file);
    byte header[13];
    zox_png_u32(header, (uint) width);
    zox_png_u32(header + 4, (uint) height);
    header[8] = 8;      // bits a channel
    header[9] = 2;      // rgb
    header[10] = 0;
    header[11] = 0;
    header[12] = 0;
    zox_png_chunk(file, "IHDR", header, 13);
    zox_png_chunk(file, "IDAT", z, (uint) at);
    zox_png_chunk(file, "IEND", NULL, 0);
    fclose(file);
    free(z);
    return 1;
}
