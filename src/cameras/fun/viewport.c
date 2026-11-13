const float4 single_screen_to_canvas = (float4) { 1, 1, 0, 0 };

// a 0 to 1 edge of the screen to pixels, rounded so neighbouring viewports share the edge
static inline int screen_to_canvas_edge(int size, float t) {
    return (int) floorf(t * size + 0.5f);
}

// a viewport's pixel size, its part (screen_to_canvas) of the screen size
//  - from rounded edges, so a grid of viewports covers every pixel (1001 wide: 500 + 501)
int2 screen_to_canvas_size(int2 size, float4 screen_to_canvas) {
    const int2 start = {
        screen_to_canvas_edge(size.x, screen_to_canvas.z),
        screen_to_canvas_edge(size.y, screen_to_canvas.w) };
    const int2 end = {
        screen_to_canvas_edge(size.x, screen_to_canvas.z + screen_to_canvas.x),
        screen_to_canvas_edge(size.y, screen_to_canvas.w + screen_to_canvas.y) };
    return (int2) { end.x - start.x, end.y - start.y };
}

// a viewport's pixel position (bottom left, gl), its part (screen_to_canvas) of the screen size
int2 screen_to_canvas_position(int2 size, float4 screen_to_canvas) {
    return (int2) {
        screen_to_canvas_edge(size.x, screen_to_canvas.z),
        screen_to_canvas_edge(size.y, screen_to_canvas.w) };
}

// a cell of a columns x rows grid over the screen, as a screen_to_canvas (size xy, position zw, 0 to 1)
//  - index 0 is the top left, then along the row and down (gl viewports start at the bottom left)
float4 viewport_grid_cell(
    int index,
    int columns,
    int rows)
{
    if (columns <= 0 || rows <= 0) {
        return single_screen_to_canvas;
    }
    const int column = index % columns;
    const int row = (index / columns) % rows;
    return (float4) {
        1 / (float) columns,
        1 / (float) rows,
        column / (float) columns,
        (rows - 1 - row) / (float) rows
    };
}

// the pixel size a camera draws at for its part of the window
//  - render cameras draw to a texture, scaled down by viewport_downscale (web), the ui stretches it back
//  - cameras drawing straight to the screen keep the full size (else they'd fill only a corner of it)
int2 camera_viewport_size(
    ecs* world,
    entity camera,
    int2 size)
{
    if (zox_has(camera, RenderCamera)) {
        return scale_viewport(size);
    }
    return size;
}
