// tst: font_fill, texture_fill_shapes fills drawn outlines, pixels away from lines match the real shapes
//  - squares, a ring (its hole stays empty), an A (an apex on the scan row), a diamond and an open U (no fill)
//  - run: bash bsh/tst.sh font_fill
// tst: font_glyphs_fill, every glyph of the font at 32 and 64 pixels against its outline geometry
//  - pixels more than 1.5 from a line are filled if inside the glyph (font_winding), air if outside
//  - glyphs of lines only (an end no other segment meets) fill nothing
//  - saves tst/font_fill.ppm, the glyphs in a grid, to look at
//  - run: bash bsh/tst.sh font_glyphs_fill
#define zox_tst_fill_air ((color) { 0, 0, 0, 0 })
#define zox_tst_fill_line ((color) { 255, 0, 0, 255 })
#define zox_tst_fill_color ((color) { 255, 255, 255, 255 })

// lines only: an end that no other segment meets, an open glyph has no inside to fill
byte zox_tst_fill_open(const byte2* points, int length) {
    for (int i = 0; i < length; i++) {
        int meets = 0;
        for (int j = 0; j < length; j++) {
            meets += points[j].x == points[i].x && points[j].y == points[i].y;
        }
        if (meets & 1) {
            return 1;
        }
    }
    return 0;
}

float zox_tst_fill_line_distance(float2 p, const byte2* points, int length, int2 size, float2 pad) {
    float nearest = 1e9f;
    for (int i = 0; i + 1 < length; i += 2) {
        const float2 a = font_segment_point(points[i], size, pad);
        const float2 b = font_segment_point(points[i + 1], size, pad);
        const float2 ab = { b.x - a.x, b.y - a.y };
        const float ab2 = ab.x * ab.x + ab.y * ab.y;
        float t = ab2 > 0 ? ((p.x - a.x) * ab.x + (p.y - a.y) * ab.y) / ab2 : 0;
        t = t < 0 ? 0 : (t > 1 ? 1 : t);
        const float dx = p.x - (a.x + ab.x * t), dy = p.y - (a.y + ab.y * t);
        const float d = sqrtf(dx * dx + dy * dy);
        nearest = d < nearest ? d : nearest;
    }
    return nearest;
}

void zox_tst_fill_draw(color* data, int2 size, const byte2* points, int length, float2 pad) {
    clear_texture(data, size, zox_tst_fill_air);
    generate_font_lines(data, size, points, length, zox_tst_fill_line, pad);
    texture_fill_shapes(data, size, points, length, pad, zox_tst_fill_air, zox_tst_fill_line, zox_tst_fill_color);
}

// pixels away from the lines that are filled outside the shape, or left air inside it, any fill in an open shape
int zox_tst_fill_wrong(const color* data, int2 size, const byte2* points, int length, float2 pad, int* filled) {
    const byte open = zox_tst_fill_open(points, length);
    int wrong = 0;
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            const float2 p = { x + 0.5f, y + 0.5f };
            const color c = data[y * size.x + x];
            const byte is_fill = color_equal(c, zox_tst_fill_color);
            *filled += is_fill;
            wrong += open && is_fill;
            if (open || zox_tst_fill_line_distance(p, points, length, size, pad) <= 1.5f) {
                continue;
            }
            const byte inside = font_winding(p, points, length, size, pad) != 0;
            wrong += inside ? !color_equal(c, zox_tst_fill_color) : !color_equal(c, zox_tst_fill_air);
        }
    }
    return wrong;
}

byte zox_tst_font_fill_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_font_fill_check(ecs* world) {
    (void) world;
    const float2 pad = float2_zero;
    const int2 size = int2_single(32);
    color data[32 * 32];
    // each shape is segment pairs, in font space 0 to 255
    const byte2 square[] = { {40,40},{200,40}, {200,40},{200,200}, {200,200},{40,200}, {40,200},{40,40} };
    const byte2 ring[] = { {30,30},{220,30}, {220,30},{220,220}, {220,220},{30,220}, {30,220},{30,30},
        {90,90},{90,160}, {90,160},{160,160}, {160,160},{160,90}, {160,90},{90,90} };   // a hole winds the other way
    const byte2 letter_a[] = { {128,20},{230,230}, {230,230},{180,230}, {180,230},{128,110}, {128,110},{76,230},
        {76,230},{26,230}, {26,230},{128,20} };
    const byte2 diamond[] = { {128,16},{240,128}, {240,128},{128,240}, {128,240},{16,128}, {16,128},{128,16} };
    const byte2 open_u[] = { {40,40},{40,220}, {40,220},{220,220}, {220,220},{220,40} };
    // a part walked back over (a bridge to an inner box) and a box overlapping it, as the font's glyphs do
    const byte2 bridged[] = { {30,30},{220,30}, {220,30},{220,220}, {220,220},{30,220}, {30,220},{30,30},
        {30,30},{90,90}, {90,90},{90,160}, {90,160},{160,160}, {160,160},{160,90}, {160,90},{90,90}, {90,90},{30,30} };
    const struct { const char* name; const byte2* points; int length; int2 pixel; byte fill; } shapes[] = {
        { "square", square, 8, { 16, 16 }, 1 },
        { "ring", ring, 16, { 16, 16 }, 0 },
        { "letter_a", letter_a, 12, { 16, 26 }, 0 },
        { "diamond", diamond, 8, { 16, 16 }, 1 },
        { "bridged", bridged, 20, { 16, 16 }, 0 },
        { "open_u", open_u, 6, { 16, 16 }, 0 },
    };
    for (int s = 0; s < (int) (sizeof(shapes) / sizeof(shapes[0])); s++) {
        zox_tst_fill_draw(data, size, shapes[s].points, shapes[s].length, pad);
        int filled = 0;
        const int wrong = zox_tst_fill_wrong(data, size, shapes[s].points, shapes[s].length, pad, &filled);
        zox_tst_check(!wrong, "[%s] [%i] pixels filled wrong", shapes[s].name, wrong);
        zox_tst_check(filled || zox_tst_fill_open(shapes[s].points, shapes[s].length), "[%s] nothing filled", shapes[s].name);
        // a known pixel: the square's and diamond's middle, the ring's hole, under the A's notch
        const color c = data[shapes[s].pixel.y * size.x + shapes[s].pixel.x];
        zox_tst_check(color_equal(c, shapes[s].fill ? zox_tst_fill_color : zox_tst_fill_air),
            "[%s] pixel [%ix%i] should be %s", shapes[s].name, shapes[s].pixel.x, shapes[s].pixel.y, shapes[s].fill ? "filled" : "air");
    }
}

void zox_tst_fill_save(const char* path, const color* grid, int2 size) {
    FILE* file = fopen(path, "wb");
    if (!file) {
        return;
    }
    fprintf(file, "P6\n%i %i\n255\n", size.x, size.y);
    for (int i = 0; i < size.x * size.y; i++) {
        const byte rgb[3] = { grid[i].r, grid[i].g, grid[i].b };
        fwrite(rgb, 1, 3, file);
    }
    fclose(file);
}

byte zox_tst_font_glyphs_fill_ready(ecs* world) {
    const entity style = get_font_style_using();
    if (!zox_valid(style) || !zox_get_children_count(world, style)) {
        zox_tst_waiting = "the font style";
        return 0;
    }
    return 1;
}

void zox_tst_font_glyphs_fill_check(ecs* world) {
    entity fonts[256];
    const uint count = zox_get_children(world, get_font_style_using(), fonts, 256);
    const int sizes[] = { 32, 64 };
    const int columns = 16;
    const int rows = (count + columns - 1) / columns;
    const int2 grid_size = { columns * 64, rows * 64 };
    color* grid = calloc(grid_size.x * grid_size.y, sizeof(color));
    color* data = malloc(64 * 64 * sizeof(color));
    int glyphs = 0, wrong_glyphs = 0, wrong_pixels = 0;
    for (uint f = 0; f < count; f++) {
        if (!zox_valid(fonts[f]) || !zox_has(fonts[f], FontData)) {
            continue;
        }
        const FontData* font_data = zox_get(fonts[f], FontData);
        if (!font_data->length) {
            continue;
        }
        glyphs++;
        int glyph_wrong = 0;
        for (int s = 0; s < 2; s++) {
            const int2 size = int2_single(sizes[s]);
            zox_tst_fill_draw(data, size, font_data->value, font_data->length, font_point_padding);
            int filled = 0;
            glyph_wrong += zox_tst_fill_wrong(data, size, font_data->value, font_data->length, font_point_padding, &filled);
            if (sizes[s] == 64) {
                const int gx = (f % columns) * 64, gy = (f / columns) * 64;
                for (int y = 0; y < 64; y++) {
                    for (int x = 0; x < 64; x++) {
                        grid[(gy + y) * grid_size.x + gx + x] = data[y * 64 + x];
                    }
                }
            }
        }
        if (glyph_wrong) {
            wrong_glyphs++;
            wrong_pixels += glyph_wrong;
            zox_log("[tst] font_glyphs_fill glyph [%i] has [%i] pixels filled wrong", f, glyph_wrong);
        }
    }
    zox_tst_fill_save(zox_tst_path("font_fill.ppm"), grid, grid_size);
    free(grid);
    free(data);
    zox_log("[tst] [metric] font_glyphs_fill [%i] glyphs, [%i] filled wrong, [%i] pixels", glyphs, wrong_glyphs, wrong_pixels);
    zox_tst_check(glyphs, "no glyphs in the font");
    zox_tst_check(!wrong_glyphs, "[%i] glyphs filled wrong, [%i] pixels", wrong_glyphs, wrong_pixels);
}

// tst: texture_lines, draw_texture_line hits both ends, steps 8 sides with no gaps, one pixel a step
//  - run: bash bsh/tst.sh texture_lines
void zox_tst_texture_lines_check(ecs* world) {
    (void) world;
    const int2 size = int2_single(32);
    color data[32 * 32];
    const int2 lines[][2] = { { { 0, 0 }, { 31, 31 } }, { { 3, 28 }, { 29, 5 } }, { { 30, 2 }, { 1, 9 } },
        { { 16, 0 }, { 16, 31 } }, { { 0, 16 }, { 31, 16 } }, { { 7, 7 }, { 7, 7 } }, { { 2, 30 }, { 9, 1 } } };
    for (int l = 0; l < (int) (sizeof(lines) / sizeof(lines[0])); l++) {
        const int2 a = lines[l][0], b = lines[l][1];
        clear_texture(data, size, zox_tst_fill_air);
        draw_texture_line(data, size, a, b, zox_tst_fill_line);
        int count = 0, lonely = 0;
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 32; x++) {
                if (!color_equal(data[y * 32 + x], zox_tst_fill_line)) {
                    continue;
                }
                count++;
                // every pixel but a lone dot has a neighbour (8 sides)
                int neighbours = 0;
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        const int nx = x + dx, ny = y + dy;
                        neighbours += (dx || dy) && nx >= 0 && ny >= 0 && nx < 32 && ny < 32 &&
                            color_equal(data[ny * 32 + nx], zox_tst_fill_line);
                    }
                }
                lonely += !neighbours;
            }
        }
        const int steps = int_max(abs(b.x - a.x), abs(b.y - a.y)) + 1;
        zox_tst_check(color_equal(data[a.y * 32 + a.x], zox_tst_fill_line) && color_equal(data[b.y * 32 + b.x], zox_tst_fill_line),
            "line [%i] misses an end", l);
        zox_tst_check(count == steps, "line [%i] has [%i] pixels, expected [%i]", l, count, steps);
        zox_tst_check(steps == 1 || !lonely, "line [%i] has [%i] gaps", l, lonely);
    }
}

void zox_tsts_glyphs() {
    zox_tst_add("texture_lines", zox_tst_font_fill_ready, zox_tst_texture_lines_check, 30);
    zox_tst_add("font_fill", zox_tst_font_fill_ready, zox_tst_font_fill_check, 30);
    zox_tst_add("font_glyphs_fill", zox_tst_font_glyphs_fill_ready, zox_tst_font_glyphs_fill_check, 30);
}
