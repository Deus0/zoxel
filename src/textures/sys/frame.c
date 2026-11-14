// Frame textures (windows, headers, buttons, panels): a fill inside an outline, its corners cut
//  - the fill a soft gradient, lighter at its top, darker at its bottom
//  - a bevel just inside the outline: a highlight along its top and left, a shadow along its bottom and right, raised
//  - a thin inner rule inset from the outline, its cut corners a lighter accent
//  - noise over it (TextureAddNoise), a little grain
#define frame_gradient 0.12f       // the fill lighter and darker by this at its top and bottom
#define frame_bevel_light 1.25f
#define frame_bevel_dark 0.75f
#define frame_rule_shade 1.2f      // the inner rule, the outline lighter

static inline color frame_shade(color c, float shade) {
    return (color) {
        (byte) fminf(255, c.r * shade),
        (byte) fminf(255, c.g * shade),
        (byte) fminf(255, c.b * shade),
        c.a
    };
}

static inline color frame_noise(color c, int amount) {
    return (color) {
        (byte) int_min(255, c.r + rand() % amount),
        (byte) int_min(255, c.g + rand() % amount),
        (byte) int_min(255, c.b + rand() % amount),
        c.a
    };
}

void generate_texture_frame(color* data, int2 size, color fill_color, color outline_color, byte frame_thickness, byte corner_size, byte is_noise) {
    const int width = size.x, height = size.y;
    const int rule = frame_thickness + 2;       // the inner rule, two pixels in from the outline
    const color rule_color = frame_shade(outline_color, frame_rule_shade);
    int index = 0;
    for (int y = 0; y < height; y++) {
        const int by = height - 1 - y;
        for (int x = 0; x < width; x++, index++) {
            const int rx = width - 1 - x;
            const int corner_distance = int_min(int_min(x + y, rx + y), int_min(rx + by, x + by));
            // how far in from its edge
            const int in = int_min(int_min(x, y), int_min(rx, by));
            color c;
            if (corner_distance < corner_size) {
                c = empty_color;
            } else if (in < frame_thickness || corner_distance < corner_size + frame_thickness) {
                // its outline, its cut corners a lighter accent
                c = corner_distance < corner_size + frame_thickness ? frame_shade(outline_color, frame_rule_shade) : outline_color;
                if (is_noise) {
                    c = frame_noise(c, 25);
                }
            } else {
                // its fill: lighter at its top (y 0) to darker at its bottom
                const float t = height > 1 ? (float) y / (height - 1) : 0;
                float shade = 1 + frame_gradient * (1 - 2 * t);
                // the bevel, just inside the outline
                if (in == frame_thickness) {
                    shade *= (y == frame_thickness || x == frame_thickness) ? frame_bevel_light : frame_bevel_dark;
                }
                c = frame_shade(fill_color, shade);
                // the inner rule, inset from the outline (big enough frames)
                if (in == rule && width > rule * 4 && height > rule * 4 && corner_distance >= corner_size + rule) {
                    c = rule_color;
                }
                if (is_noise) {
                    c = frame_noise(c, 55);
                }
            }
            data[index] = c;
        }
    }
}

void frame_texture_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(FillColor);
    zox_sys_in(OutlineColor);
    zox_sys_in(OutlineThickness);
    zox_sys_in(FrameCorner);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureSize, size);
        zox_sys_i(FillColor, fill);
        zox_sys_i(OutlineColor, outline);
        zox_sys_i(OutlineThickness, thickness);
        zox_sys_i(FrameCorner, edge);
        zox_sys_o(GenerateTexture, generate);
        zox_sys_o(TextureData, data);
        if (generate->value != zox_generate_texture_run) {
            continue;
        }
        uint length = size->value.x * size->value.y;
        if (!length) {
            continue;
        }
        byte add_noise = zox_has(e, TextureAddNoise);
        resize_TextureData(data, length);
        generate_texture_frame(data->value, size->value, fill->value, outline->value, thickness->value, edge->value, add_noise);
        generate->value = zox_generate_texture_end;
        zox_add(e, TextureDirty);
        zox_sys_increment();
        if (dbg_log) {
            zox_log("Frame Texture generated [%s] fill [%ix%ix%ix%i] Size [%ix%i]", zox_get_name(e), fill->value.r, fill->value.g, fill->value.b, fill->value.a, size->value.x, size->value.y);
        }
    }
    // zox_log("Frame Textures Generated [%i]:[%f]", process_count, calculate_sys_delta());
    zox_sys_on_end();
} zoxd_system(frame_texture_system);
