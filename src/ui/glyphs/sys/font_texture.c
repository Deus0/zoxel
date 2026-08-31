const uint safety_checks_floodfill = 2000;

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

byte texture_does_flood_reach_edge(
    const color* pixels,
    const int2 size,
    const color air_color,
    const color boundary_color,
    const int start_x,
    const int start_y
) {
    if (size.x <= 0 || size.y <= 0) {
        return 0;
    }

    if (start_x < 0 || start_x >= size.x ||
        start_y < 0 || start_y >= size.y) {
        return 0;
    }

    const int start_index =
        int2_array_index((int2) { start_x, start_y }, size);

    if (!color_equal(pixels[start_index], air_color)) {
        return 0;
    }

    const int pixel_count = size.x * size.y;

    byte* visited = calloc(pixel_count, sizeof(byte));
    int* stack = malloc(pixel_count * sizeof(int));

    if (!visited || !stack) {
        free(visited);
        free(stack);
        zox_logw("[texture_does_flood_reach_edge] allocation failed");
        return 0;
    }

    int stack_top = 0;

    visited[start_index] = 1;
    stack[stack_top++] = start_index;

    while (stack_top > 0) {
        const int index = stack[--stack_top];

        const int x = index % size.x;
        const int y = index / size.x;

        if (x == 0 || x == size.x - 1 ||
            y == 0 || y == size.y - 1) {
            free(visited);
            free(stack);
            return 1;
        }

        const int neighbours[4] = {
            index - 1,
            index + 1,
            index - size.x,
            index + size.x
        };

        for (int i = 0; i < 4; i++) {
            const int neighbour = neighbours[i];

            if (visited[neighbour]) {
                continue;
            }

            const int nx = neighbour % size.x;
            const int ny = neighbour / size.x;

            if (nx < 0 || nx >= size.x ||
                ny < 0 || ny >= size.y) {
                continue;
            }

            if (!color_equal(pixels[neighbour], air_color)) {
                continue;
            }

            visited[neighbour] = 1;
            stack[stack_top++] = neighbour;
        }
    }

    free(visited);
    free(stack);
    return 0;
}
void flood_fill_texture(
    color* pixels,
    const int2 size,
    const color air_color,
    const color boundary_color,
    const color fill_color,
    const int start_x,
    const int start_y
) {
    if (size.x <= 0 || size.y <= 0) {
        return;
    }

    if (start_x < 0 || start_x >= size.x ||
        start_y < 0 || start_y >= size.y) {
        return;
    }

    const int start_index =
        int2_array_index((int2) { start_x, start_y }, size);

    if (!color_equal(pixels[start_index], air_color)) {
        return;
    }

    const int pixel_count = size.x * size.y;

    byte* visited = calloc(pixel_count, sizeof(byte));
    int* stack = malloc(pixel_count * sizeof(int));

    if (!visited || !stack) {
        free(visited);
        free(stack);
        zox_logw("[flood_fill_texture] allocation failed");
        return;
    }

    int stack_top = 0;

    visited[start_index] = 1;
    stack[stack_top++] = start_index;

    while (stack_top > 0) {
        const int index = stack[--stack_top];

        if (color_equal(pixels[index], boundary_color)) {
            continue;
        }

        if (!color_equal(pixels[index], air_color)) {
            continue;
        }

        pixels[index] = fill_color;

        const int x = index % size.x;
        const int y = index / size.x;

        if (x > 0) {
            const int n = index - 1;
            if (!visited[n] && color_equal(pixels[n], air_color)) {
                visited[n] = 1;
                stack[stack_top++] = n;
            }
        }

        if (x + 1 < size.x) {
            const int n = index + 1;
            if (!visited[n] && color_equal(pixels[n], air_color)) {
                visited[n] = 1;
                stack[stack_top++] = n;
            }
        }

        if (y > 0) {
            const int n = index - size.x;
            if (!visited[n] && color_equal(pixels[n], air_color)) {
                visited[n] = 1;
                stack[stack_top++] = n;
            }
        }

        if (y + 1 < size.y) {
            const int n = index + size.x;
            if (!visited[n] && color_equal(pixels[n], air_color)) {
                visited[n] = 1;
                stack[stack_top++] = n;
            }
        }
    }

    free(visited);
    free(stack);
}

// NOTE: Moved [stack, visited] to stack from heap, as heap had limits on these sizes

/*byte texture_does_flood_reach_edge(
    const color* pixels,
    const int2 size,
    const color air_color,
    const color boundary_color,
    const int start_x,
    const int start_y
) {
    if (!size.x || !size.y) {
        return 0;
    }

    if (start_x < 0 || start_x >= size.x ||
        start_y < 0 || start_y >= size.y) {
        return 0;
    }

    int start_index = int2_array_index((int2) { start_x, start_y }, size);
    if (!color_equal(pixels[start_index], air_color)) {
        return 0;
    }

    int pixel_count = size.x * size.y;

    int *visited = calloc(pixel_count, sizeof(int));
    if (!visited) {
        zox_logw("[texture_does_flood_reach_edge] calloc failed");
        return 0;
    }

    const int stack_cap = pixel_count * 8;
    int *stack = malloc(stack_cap * sizeof(int));
    if (!stack) {
        zox_logw("[texture_does_flood_reach_edge] malloc failed");
        free(visited);
        return 0;
    }

    int stack_top = 0;
    stack[stack_top++] = start_x; // x-coordinate
    stack[stack_top++] = start_y; // y-coordinate

    // Loop until the stack is empty
    uint checks = 0;
    byte reached_edge = 0;

    while (stack_top > 0 && checks++ < safety_checks_floodfill) {

        // Pop the top pixel from the stack
        int y = stack[--stack_top];
        int x = stack[--stack_top];

        if (x < 0 || x >= size.x || y < 0 || y >= size.y) {
            continue;
        }

        int index = int2_array_index((int2){ x, y }, size);
        if (visited[index]) {
            continue;
        }

        visited[index] = 1;

        if (color_equal(pixels[index], boundary_color) || !color_equal(pixels[index], air_color)) {
            continue;
        }

        if (x == 0 || x == size.x - 1 ||
            y == 0 || y == size.y - 1) {
            reached_edge = 1;
            break;
        }

        if (stack_top + 8 >= stack_cap) {
            break;
        }

        stack[stack_top++] = x - 1; stack[stack_top++] = y;
        stack[stack_top++] = x + 1; stack[stack_top++] = y;
        stack[stack_top++] = x;     stack[stack_top++] = y - 1;
        stack[stack_top++] = x;     stack[stack_top++] = y + 1;
    }

    free(visited);
    free(stack);
    return reached_edge;
}

void flood_fill_texture(
    color* pixels,
    const int2 size,
    const color air_color,
    const color boundary_color,
    const color fill_color,
    const int start_x,
    const int start_y
) {
    if (!size.x || !size.y) {
        zox_logw("[flood_fill_texture] No Size");
        return;
    }

    int index = int2_array_index((int2) { start_x, start_y }, size);
    if (!color_equal(pixels[index], air_color)) {
        return;
    }

    int pixel_count = size.x * size.y;

    int *visited = calloc(pixel_count, sizeof(int));
    if (!visited) {
        zox_logw("[flood_fill_texture] calloc failed");
        return;
    }

    int *stack = malloc(pixel_count * 8 * sizeof(int));
    if (!stack) {
        zox_logw("[flood_fill_texture] malloc failed");
        free(visited);
        return;
    }

    int stack_top = 0;
    stack[stack_top++] = start_x; // x-coordinate
    stack[stack_top++] = start_y; // y-coordinate

    // Loop until the stack is empty
    uint checks = 0;
    while (stack_top > 0 && checks < safety_checks_floodfill) {
        // Pop the top pixel from the stack
        int y = stack[--stack_top];
        int x = stack[--stack_top];
        // Check if the pixel is within the texture bounds and hasn't been visited
        index = int2_array_index((int2) { x, y }, size);
        if (x >= 0 && x < size.x && y >= 0 && y < size.y && !visited[index]) {
            visited[index] = 1;
            if (color_equal(pixels[index], boundary_color) ||color_equal(pixels[index], fill_color)) {
                continue;
            }
            pixels[index] = fill_color;
            // zox_log("filling [%ix%i]\n", x, y)
            // Push neighboring pixels onto the stack (left, right, up, down)
            stack[stack_top++] = x - 1; // left
            stack[stack_top++] = y;
            stack[stack_top++] = x + 1; // right
            stack[stack_top++] = y;
            stack[stack_top++] = x;     // up
            stack[stack_top++] = y - 1;
            stack[stack_top++] = x;     // down
            stack[stack_top++] = y + 1;
        }
        checks++;
    }
    free(visited);
    free(stack);
}*/

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


// raycast from all 4 sides!
/*void scanline_fill_texture(
    color* data,
    const int2 size,
    const color air_color,
    const color boundary_color,
    const color fill_color
) {
    int2 last_boundary_pixel = int2_zero;
    for (int y = 0; y < size.y; y++) {
        byte intersects = 0;
        byte found_filling = 0;
        for (int x = 0; x < size.x; x++) {
            const int index = int2_array_index((int2) { x, y }, size);
            if (color_equal(data[index], boundary_color)) {
                if (intersects && found_filling) {
                    byte hits_edge = texture_does_flood_reach_edge(
                        data,
                        size,
                        air_color,
                        boundary_color,
                        last_boundary_pixel.x,
                        last_boundary_pixel.y
                    );
                    if (!hits_edge) {
                        flood_fill_texture(
                            data,
                            size,
                            air_color,
                            boundary_color,
                            fill_color,
                            last_boundary_pixel.x,
                            last_boundary_pixel.y);
                    }
                }
                found_filling = 0;
                intersects = !intersects;
                if (!intersects) {
                    break;
                }
            } else {
                // find position in air
                if (!found_filling) {
                    found_filling = 1;
                    last_boundary_pixel = (int2) { x, y };
                }
            }
        }
    }
    // texture_swap_colors(data, size, boundary_color, fill_color);
}*/


// NOTE: Only works on one interior shaape
void texture_fill_shapes(
    color* data,
    const int2 size,
    const color air_color,
    const color boundary_color,
    const color fill_color
) {

    if (size.x <= 0 || size.y <= 0) {
        return;
    }
    const int pixel_count = size.x * size.y;
    byte* outside = calloc(pixel_count, sizeof(byte));
    byte* visited = calloc(pixel_count, sizeof(byte));
    int* stack = malloc(pixel_count * sizeof(int));
    int* region = malloc(pixel_count * sizeof(int));
    if (!outside || !visited || !stack || !region) {
        free(outside);
        free(visited);
        free(stack);
        free(region);
        zox_logw("[scanline_fill_texture] allocation failed");
        return;
    }
    int stack_top = 0;
    for (int x = 0; x < size.x; x++) {
        const int top = int2_array_index((int2) { x, 0 }, size);
        if (!outside[top] && color_equal(data[top], air_color)) {
            outside[top] = 1;
            stack[stack_top++] = top;
        }
        const int bottom = int2_array_index(
            (int2) { x, size.y - 1 },
            size
        );
        if (!outside[bottom] && color_equal(data[bottom], air_color)) {
            outside[bottom] = 1;
            stack[stack_top++] = bottom;
        }
    }
    for (int y = 1; y < size.y - 1; y++) {
        const int left = int2_array_index((int2) { 0, y }, size);
        if (!outside[left] && color_equal(data[left], air_color)) {
            outside[left] = 1;
            stack[stack_top++] = left;
        }
        const int right = int2_array_index(
            (int2) { size.x - 1, y },
            size
        );
        if (!outside[right] && color_equal(data[right], air_color)) {
            outside[right] = 1;
            stack[stack_top++] = right;
        }
    }
    while (stack_top > 0) {
        const int index = stack[--stack_top];
        const int x = index % size.x;
        const int y = index / size.x;
        if (x > 0) {
            const int n = index - 1;
            if (!outside[n] && color_equal(data[n], air_color)) {
                outside[n] = 1;
                stack[stack_top++] = n;
            }
        }
        if (x + 1 < size.x) {
            const int n = index + 1;
            if (!outside[n] && color_equal(data[n], air_color)) {
                outside[n] = 1;
                stack[stack_top++] = n;
            }
        }
        if (y > 0) {
            const int n = index - size.x;
            if (!outside[n] && color_equal(data[n], air_color)) {
                outside[n] = 1;
                stack[stack_top++] = n;
            }
        }
        if (y + 1 < size.y) {
            const int n = index + size.x;
            if (!outside[n] && color_equal(data[n], air_color)) {
                outside[n] = 1;
                stack[stack_top++] = n;
            }
        }
    }
    for (int i = 0; i < pixel_count; i++) {
        if (outside[i] || visited[i] ||
            !color_equal(data[i], air_color)) {
            continue;
        }
        int region_length = 0;
        stack_top = 0;
        stack[stack_top++] = i;
        visited[i] = 1;
        while (stack_top > 0) {
            const int index = stack[--stack_top];
            region[region_length++] = index;
            const int x = index % size.x;
            const int y = index / size.x;
            if (x > 0) {
                const int n = index - 1;
                if (!outside[n] && !visited[n] &&
                    color_equal(data[n], air_color)) {
                    visited[n] = 1;
                    stack[stack_top++] = n;
                }
            }
            if (x + 1 < size.x) {
                const int n = index + 1;
                if (!outside[n] && !visited[n] &&
                    color_equal(data[n], air_color)) {
                    visited[n] = 1;
                    stack[stack_top++] = n;
                }
            }
            if (y > 0) {
                const int n = index - size.x;
                if (!outside[n] && !visited[n] &&
                    color_equal(data[n], air_color)) {
                    visited[n] = 1;
                    stack[stack_top++] = n;
                }
            }
            if (y + 1 < size.y) {
                const int n = index + size.x;
                if (!outside[n] && !visited[n] &&
                    color_equal(data[n], air_color)) {
                    visited[n] = 1;
                    stack[stack_top++] = n;
                }
            }
        }
        if (region_length == 0) {
            continue;
        }
        const int seed = region[0];
        const int sx = seed % size.x;
        const int sy = seed / size.x;
        int crossings = 0;
        for (int x = sx + 1; x < size.x; x++) {
            const int index = int2_array_index((int2) { x, sy }, size);
            if (!color_equal(data[index], boundary_color)) {
                continue;
            }
            if (x == sx + 1 ||
                !color_equal(
                    data[int2_array_index((int2) { x - 1, sy }, size)],
                    boundary_color
                )) {
                crossings++;
            }
        }
        if (crossings & 1) {
            for (int j = 0; j < region_length; j++) {
                data[region[j]] = fill_color;
            }
        }
    }
    free(outside);
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
    /*} else {
        fill_thickness++;
        generate_splotches_lines(
            data,
            size,
            points, points_length,
            line_color,
            fill_thickness,
            point_padding
        );
    }*/
}

zox_sys2(FontTextureSystem) {
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
    zox_sys_in(ZigelIndex);
    zox_sys_in(FillColor);
    zox_sys_in(OutlineColor);
    zox_sys_in(TextureSize);
    zox_sys_in(FontThickness);
    zox_sys_in(OutlineThickness);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZigelIndex, zindex);
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
        if (zox_has(e, CentredZigel)) {
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
            zox_log("[%s] Generated Zigel [%i] Font: F [%ix%ix%ix%i] O [%ix%ix%ix%i] checksum=%u",
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
} zox_sys_end(FontTextureSystem);
