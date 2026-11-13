// tst: texture_noise, noise stays within its fill colour's range, dark and bright fills don't wrap around
//  - each channel within fill -24 to +24 (clamped to 0 to 255), a full outline makes the border black
//  - run: bash bsh/tst.sh texture_noise
// tst: texture_outline, add_texture_outline rings a shape with one pixel (8 sides), the rest stays
//  - run: bash bsh/tst.sh texture_outline
byte zox_tst_textures_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_texture_noise_check(ecs* world) {
    (void) world;
    const int2 size = int2_single(16);
    color data[16 * 16];
    const color fills[] = { { 10, 200, 250, 255 }, { 128, 128, 128, 255 }, { 0, 255, 24, 255 } };
    for (int f = 0; f < 3; f++) {
        const color fill = fills[f];
        generate_texture_noise(data, size, zox_texture_dirt, zox_outline_type_none, fill);
        int outside = 0;
        for (int i = 0; i < size.x * size.y; i++) {
            const byte channels[3][2] = { { data[i].r, fill.r }, { data[i].g, fill.g }, { data[i].b, fill.b } };
            for (int c = 0; c < 3; c++) {
                const int low = int_max(channels[c][1] - 24, 0), high = int_min(channels[c][1] + 24, 255);
                outside += channels[c][0] < low || channels[c][0] > high;
            }
        }
        zox_tst_check(!outside, "fill [%i,%i,%i] [%i] channels out of its noise range", fill.r, fill.g, fill.b, outside);
    }
    generate_texture_noise(data, size, zox_texture_dirt, zox_outline_type_full, fills[1]);
    int border = 0;
    for (int i = 0; i < size.x; i++) {
        const int sides[4] = { i, (size.y - 1) * size.x + i, i * size.x, i * size.x + size.x - 1 };
        for (int s = 0; s < 4; s++) {
            border += data[sides[s]].r || data[sides[s]].g || data[sides[s]].b;
        }
    }
    zox_tst_check(!border, "[%i] border pixels not black with a full outline", border);
}

void zox_tst_texture_outline_check(ecs* world) {
    (void) world;
    const int2 size = int2_single(16);
    const color air = { 0, 0, 0, 0 }, shape = { 255, 255, 255, 255 }, outline = { 255, 0, 0, 255 };
    color data[16 * 16];
    generate_texture_fill(data, size, air);
    // a 4x4 square at 6 to 9, its ring is 5 to 10
    for (int y = 6; y < 10; y++) {
        for (int x = 6; x < 10; x++) {
            data[y * 16 + x] = shape;
        }
    }
    add_texture_outline(data, size, air, outline);
    int wrong = 0, outlined = 0;
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            const color c = data[y * 16 + x];
            const byte in_shape = x >= 6 && x < 10 && y >= 6 && y < 10;
            const byte in_ring = !in_shape && x >= 5 && x < 11 && y >= 5 && y < 11;
            outlined += color_equal(c, outline);
            wrong += !color_equal(c, in_shape ? shape : (in_ring ? outline : air));
        }
    }
    zox_tst_check(!wrong, "[%i] pixels wrong around the outlined square", wrong);
    zox_tst_check(outlined == 20, "[%i] outline pixels, a 4x4 square's ring is 20", outlined);
}

void zox_tsts_textures() {
    zox_tst_add("texture_noise", zox_tst_textures_ready, zox_tst_texture_noise_check, 30);
    zox_tst_add("texture_outline", zox_tst_textures_ready, zox_tst_texture_outline_check, 30);
}
