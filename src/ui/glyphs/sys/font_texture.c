const uint safety_checks_drawline = 1000;

int2 font_point_to_pixel(byte2 point, int2 size, float2 pad) {
    float2 pointf = float2_divide1(byte2_to_float2(point), 255.0f);
    pointf.x = pad.x + (1.0f - pad.x * 2) * pointf.x;
    pointf.y = pad.y + (1.0f - pad.y * 2) * pointf.y;
    return (int2) {
        (int) (pointf.x * size.x),
        (int) (pointf.y * size.y),
    };
}

void draw_texture_line(
    color* data,
    int2 size,
    int2 point_a,
    int2 point_b,
    color line_color)
{
    // Calculate the texture coordinates for point_a and point_b
    int x0 = (int) point_a.x;
    int y0 = (int) point_a.y;
    int x1 = (int) point_b.x;
    int y1 = (int) point_b.y;
    // Calculate the difference between the x and y coordinates
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    // Calculate the direction of the line along the x and y axes
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    // Calculate the error terms
    int err = dx - dy;
    int e2;
    // Loop through each pixel along the line segment
    uint checks = 0;
    while (1 && checks < safety_checks_drawline) {
        // Calculate the index of the current pixel in the texture data array
        int index = (y0 * size.x + x0);
        // Set the RGBA values of the current pixel to white (255, 255, 255, 255)
        data[index] = line_color;
        // Check if we've reached the end of the line segment
        if (x0 == x1 && y0 == y1) break;
        // Calculate the next pixel coordinates
        e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; } // Adjust the x-coordinate
        if (e2 < dx) { err += dx; y0 += sy; } // Adjust the y-coordinate
        checks++;
    }
}

void generate_splotches_lines(
    color* data,
    int2 size,
    const byte2* points,
    int points_length,
    color line_color,
    byte splotch_size,
    float2 padding)
{
    if (splotch_size == 0) {
        return;
    }
    for (int i = 0; i < points_length; i += 2) {
        int2 pointA = font_point_to_pixel(points[i], size, padding);
        int2 pointB = font_point_to_pixel(points[i + 1], size, padding);
        int distance = int2_distance(pointA, pointB);
        float2 direction = float2_normalize(float2_sub(int2_to_float2(pointB), int2_to_float2(pointA)));
#ifdef debug_font_texture
        zox_log("Font Data %i %ix%i > %ix%i\n", i, pointA.x, pointA.y, pointB.x, pointB.y)
        zox_log("    - %ix%i > %ix%i\n", pointA.x, pointA.y, pointB.x, pointB.y);
        zox_log("    - distance %i direction %fx%f\n", distance, direction.x, direction.y);
#endif
        int2 splash_point = pointA;
        for (int j = 0; j <= distance; j++) {
            int2 splash_point_check = int2_add(float2_to_int2(float2_multiply_float(direction, (float) j)), pointA);
            if (int2_equal(splash_point, splash_point_check)) continue;
            splash_point = splash_point_check;
            // add noise later to this
            int pointSize2 = splotch_size - 1; // rand() % splotch_size; // math.floor(random.NextFloat(pointSize.x, pointSize.y));
            // get surrounding pixels
            for (int k = -pointSize2; k <= pointSize2; k++) {
                for (int l = -pointSize2; l <= pointSize2; l++) {
                    const int2 drawPoint = (int2) { splash_point.x + k, splash_point.y + l};
                    if (drawPoint.x >= 0 && drawPoint.x < size.x && drawPoint.y >= 0 && drawPoint.y < size.y) {
                        const int array_index = int2_array_index(drawPoint, size);
                        if (color_equal(data[array_index], nothing_font_color)) data[array_index] = line_color;
                    }
                }
            }
            j += int_clamp(pointSize2, 0, pointSize2 - 1); // (int) math.clamp(pointSize2, 0, pointSize2 - 1);
        }
    }
}

void generate_font_lines(
    color* data,
    int2 size,
    const byte2* points,
    int points_length,
    color line_color,
    float2 padding)
{
    // point A to B - use FontData byte2 data.
    for (int i = 0; i < points_length; i += 2) {
        int2 pointA = font_point_to_pixel(points[i], size, padding);
        int2 pointB = font_point_to_pixel(points[i + 1], size, padding);
        draw_texture_line(
            data,
            size,
            pointA,
            pointB,
            line_color);
#ifdef debug_font_texture
        zox_log("-> fonting: %i %ix%i > %ix%i", i, pointA.x, pointA.y, pointB.x, pointB.y);
#endif
    }
}

static inline void centre_font_data(
    const byte2 *input,
    int count,
    byte2 *output)
{
    if (!count) {
        return;
    }
    byte min_x = 255, min_y = 255;
    byte max_x = 0, max_y = 0;
    for (int i = 0; i < count; i++) {
        byte2 p = input[i];

        if (p.x < min_x) min_x = p.x;
        if (p.x > max_x) max_x = p.x;
        if (p.y < min_y) min_y = p.y;
        if (p.y > max_y) max_y = p.y;
    }
    int offset_x = 128 - ((int)min_x + (int)max_x) / 2;
    int offset_y = 128 - ((int)min_y + (int)max_y) / 2;
    for (int i = 0; i < count; i++) {
        int x = input[i].x + offset_x;
        int y = input[i].y + offset_y;
        if (x < 0) x = 0;
        else if (x > 255) x = 255;
        if (y < 0) y = 0;
        else if (y > 255) y = 255;
        output[i].x = (byte)x;
        output[i].y = (byte)y;
    }
}

void clear_texture(color* data, int2 size, color clear) {
    int index = 0;
    for (int k = 0; k < size.y; k++) {
        for (int j = 0; j < size.x; j++) {
            data[index] = clear;
            index++;
        }
    }
}

// a segment end in pixel space, at the centre of the pixel draw_texture_line puts it on
static inline float2 font_segment_point(byte2 point, int2 size, float2 pad) {
    const int2 p = font_point_to_pixel(point, size, pad);
    return (float2) { p.x + 0.5f, p.y + 0.5f };
}

// nonzero winding of the glyph's segments around a point, overlapping parts add up, a part walked back cancels
int font_winding(float2 p, const byte2* points, int points_length, int2 size, float2 pad) {
    int winding = 0;
    for (int i = 0; i + 1 < points_length; i += 2) {
        const float2 a = font_segment_point(points[i], size, pad);
        const float2 b = font_segment_point(points[i + 1], size, pad);
        if ((a.y > p.y) == (b.y > p.y)) {
            continue;
        }
        const float x = a.x + (p.y - a.y) * (b.x - a.x) / (b.y - a.y);
        if (p.x < x) {
            winding += b.y > a.y ? 1 : -1;
        }
    }
    return winding;
}

// fills the air the glyph's lines close in, a region at a time
//  - a region touching the texture's edge is outside, an open glyph (lines only) fills nothing
//  - otherwise most of its pixels inside the glyph (font_winding) fill it, counting crossings on the raster
//    went wrong where an outline walks back over itself to reach an inner part
//  - lines with no air beside them are inside the glyph, they become fill
void texture_fill_shapes(
    color* data,
    const int2 size,
    const byte2* points,
    const int points_length,
    const float2 pad,
    const color air_color,
    const color line_color,
    const color fill_color)
{
    const int pixel_count = size.x * size.y;
    if (pixel_count <= 0) {
        return;
    }
    byte* visited = calloc(pixel_count, sizeof(byte));
    int* stack = malloc(pixel_count * sizeof(int));
    int* region = malloc(pixel_count * sizeof(int));
    if (!visited || !stack || !region) {
        free(visited);
        free(stack);
        free(region);
        zox_logw("[texture_fill_shapes] allocation failed");
        return;
    }
    const int2 sides[4] = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
    for (int i = 0; i < pixel_count; i++) {
        if (visited[i] || !color_equal(data[i], air_color)) {
            continue;
        }
        int region_length = 0, stack_top = 0;
        byte edge = 0;
        stack[stack_top++] = i;
        visited[i] = 1;
        while (stack_top > 0) {
            const int index = stack[--stack_top];
            region[region_length++] = index;
            const int x = index % size.x, y = index / size.x;
            edge |= x == 0 || y == 0 || x == size.x - 1 || y == size.y - 1;
            for (int s = 0; s < 4; s++) {
                const int nx = x + sides[s].x, ny = y + sides[s].y;
                if (nx < 0 || ny < 0 || nx >= size.x || ny >= size.y) {
                    continue;
                }
                const int n = ny * size.x + nx;
                if (!visited[n] && color_equal(data[n], air_color)) {
                    visited[n] = 1;
                    stack[stack_top++] = n;
                }
            }
        }
        if (edge) {
            continue;
        }
        int inside = 0;
        for (int j = 0; j < region_length; j++) {
            const float2 p = { region[j] % size.x + 0.5f, region[j] / size.x + 0.5f };
            inside += font_winding(p, points, points_length, size, pad) != 0;
        }
        if (inside * 2 > region_length) {
            for (int j = 0; j < region_length; j++) {
                data[region[j]] = fill_color;
            }
        }
    }
    // lines inside the glyph, no air around them (8 sides, the texture's edge counts as air)
    int inner_length = 0;
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            const int index = y * size.x + x;
            if (!color_equal(data[index], line_color)) {
                continue;
            }
            byte air = 0;
            for (int dy = -1; dy <= 1 && !air; dy++) {
                for (int dx = -1; dx <= 1 && !air; dx++) {
                    const int nx = x + dx, ny = y + dy;
                    air = nx < 0 || ny < 0 || nx >= size.x || ny >= size.y ||
                        color_equal(data[ny * size.x + nx], air_color);
                }
            }
            if (!air) {
                region[inner_length++] = index;
            }
        }
    }
    for (int j = 0; j < inner_length; j++) {
        data[region[j]] = fill_color;
    }
    free(visited);
    free(stack);
    free(region);
}

void generate_font_texture(
    color* data,
    int2 size,
    const byte2* points,
    int points_length,
    color line_color,
    color fill_color,
    byte fill_thickness,
    byte outline_thickness,
    float2 point_padding,
    color clear_color
) {
    if (!points_length) {
        return;
    }
    generate_font_lines(
        data,
        size,
        points,
        points_length,
        line_color,
        point_padding);
    texture_fill_shapes(
        data,
        size,
        points,
        points_length,
        point_padding,
        clear_color,
        line_color,
        fill_color);
    // NOTE: Removes outlines
    if (outline_thickness) {
        generate_splotches_lines(
            data,
            size,
            points,
            points_length,
            line_color,
            outline_thickness,
            point_padding
        );
    } else {
        texture_swap_colors(
            data,
            size,
            line_color,
            fill_color);
    }
}

void font_texture_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_change_check();
    color clear_color = nothing_font_color;
    float2 point_padding = font_point_padding;
    entity zox_font_style;
    uint fonts_length = 0;
    uint font_children_capacity = 256;
    entity font_children[font_children_capacity];
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GlyphIndex);
    zox_sys_in(FillColor);
    zox_sys_in(OutlineColor);
    zox_sys_in(TextureSize);
    zox_sys_in(FontThickness);
    zox_sys_in(OutlineThickness);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GlyphIndex, zindex);
        zox_sys_i(TextureSize, size);
        zox_sys_i(FillColor, fill);
        zox_sys_i(OutlineColor, outline);
        zox_sys_i(FontThickness, thickness);
        zox_sys_i(OutlineThickness, outline_thickness);
        zox_sys_o(GenerateTexture, generate);
        zox_sys_o(TextureData, data);
        if (generate->value != zox_generate_texture_run) {
            continue;
        }
        // NOTE: Gets our children
        if (!fonts_length) {
            zox_font_style = get_font_style_using();
            if (!zox_valid(zox_font_style)) {
                zox_loge("[zox_font_style] is NULL");
                return;
            }
            fonts_length = zox_get_children(world, zox_font_style, font_children, font_children_capacity);
            if (!fonts_length) {
                zox_loge("[font_style_children] is NULL");
                return;
            }
        }
        if (zindex->value >= fonts_length) {
            zox_loge("Font Index out of bounds [%i] [%i]", zindex->value, fonts_length);
            continue;
        }
        // get font based on zigel index
        entity font = font_children[zindex->value];
        if (!zox_valid(font) ||
            !zox_has(font, FontData))
        {
            resize_TextureData(data, 0);
            zox_add(e, TextureDirty);
            continue;
        }
        if (font_texture_min_size &&
            size->value.x < font_texture_min_size)
        {
            zox_setv(e, TextureSize, int2_single(font_texture_min_size));
            continue;
        }
        int length = size->value.x * size->value.y;
        if (length <= 0) {
            resize_TextureData(data, 0);
            zox_add(e, TextureDirty);
            continue;
        }
        const FontData* raw_font_data = zox_get(font, FontData);
        const byte2 *font_data = (const byte2*) raw_font_data->value;
        byte2 centred_font_data[raw_font_data->length];
        if (zox_has(e, CentredGlyph)) {
            // its a byte* array with range [0, 255]
            centre_font_data(
                raw_font_data->value,
                raw_font_data->length,
                centred_font_data);
            font_data = centred_font_data;
        }
        // Create texture
        resize_TextureData(data, length);
        clear_texture(
            data->value,
            size->value,
            clear_color);
        generate_font_texture(
            data->value,
            size->value,
            font_data,
            raw_font_data->length,
            outline->value,
            fill->value,
            thickness->value,
            outline_thickness->value,
            point_padding,
            clear_color);
        generate->value = zox_generate_texture_end;
        zox_add(e, TextureDirty);
        if (dbg_log) {
            uint32_t checksum = 0;
            for (int j = 0; j < data->length; j++) {
                checksum += data->value[j].r;
                checksum += data->value[j].g;
                checksum += data->value[j].b;
                checksum += data->value[j].a;
            }
            zox_log("[%s] Generated Glyph [%i] Font: F [%ix%ix%ix%i] O [%ix%ix%ix%i] checksum=%u",
                zox_getn(e),
                zindex->value,
                fill->value.r,
                fill->value.g,
                fill->value.b,
                fill->value.a,
                outline->value.r,
                outline->value.b,
                outline->value.g,
                outline->value.a,
                checksum);
        }
    }
    zox_sys_on_end();
} zoxd_system(font_texture_system);
