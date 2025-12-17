const uint safety_checks_floodfill = 16000;

// NOTE: Moved [stack, visited] to stack from heap, as heap had limits on these sizes

byte texture_does_flood_reach_edge(
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
}