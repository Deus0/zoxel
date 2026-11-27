// tst: window_grid_resize, a grid window (an inventory's slots) resized: its columns fit its width, its rows follow,
//  it grows to fit them, it stays on its canvas (sys/layout.c window_grid_fit, window_canvas_fit)
//  - a window of 16 cells, 4 by 4, made narrow by its right handle: fewer columns, more rows, taller, every cell inside
//      its body; then wide past the canvas: as wide as the canvas, more columns, every cell inside, the window on it
//  - before: the grid kept 4 columns, its cells spilled out of a narrow window and drifted in a wide one, a window could
//      grow off its canvas
//  - antitests: without window_grid_fit it times out waiting for the narrow window's grid; without window_canvas_fit
//      and drag.c's canvas limit, the widened window is 2120 wide on a 460 canvas
//  - run: bash bsh/tst.sh window_grid_resize
#define zox_tst_grid_cells 16
#define zox_tst_grid_cell 40

// every cell inside its body, and the window inside its canvas
static void zox_tst_grid_inside(ecs* world, entity canvas, entity window, entity body, const char* when) {
    const int2 body_size = zox_getv(body, LayoutSize);
    int outside = 0, count = 0;
    iter it = zox_children(world, body);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const int2 p = zox_getv(it.entities[i], LayoutPosition);
            const int2 s = zox_getv(it.entities[i], LayoutSize);
            outside += abs(p.x) + s.x / 2 > body_size.x / 2 || abs(p.y) + s.y / 2 > body_size.y / 2;
            count++;
        }
    }
    const byte2 grid = zox_getv(body, GridSize);
    zox_tst_check(!outside && grid.x * grid.y >= count, "%s: [%i] of [%i] cells outside the [%ix%i] body, grid [%ix%i]",
        when, outside, count, body_size.x, body_size.y, grid.x, grid.y);
    const int2 canvas_size = zox_getv(canvas, LayoutSize);
    const int2 size = zox_getv(window, LayoutSize);
    const int2 position = zox_getv(window, LayoutPosition);
    zox_tst_check(abs(position.x) + size.x / 2 <= canvas_size.x / 2 && abs(position.y) + size.y / 2 <= canvas_size.y / 2,
        "%s: the [%ix%i] window at [%ix%i] is off its [%ix%i] canvas", when, size.x, size.y, position.x, position.y,
        canvas_size.x, canvas_size.y);
}

static byte zox_tst_window_grid_resize_ready(ecs* world) {
    static int step;
    static entity canvas, window, body, handle;
    static int2 before;
    static byte2 grid_before;
    int count;
    zox_tst_progress(step);
    switch (step) {
        case 0: {
            zox_tst_waiting = "the fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            const byte2 margins = byte2_single(4);
            const int side = 4 * zox_tst_grid_cell + 2 * margins.x;
            const entity3 e3 = spawn_window(world, prefab_window, prefab_grid, "Grid", canvas, int2_zero,
                (int2) { side, side }, float2_half, 12, byte2_single(4), NULL);
            window = e3.x;
            body = e3.z;
            zox_setv(body, GridSize, ((byte2) { 4, 4 }));
            zox_setv(body, GridPadding, byte2_zero);
            zox_setv(body, GridMargins, margins);
            const int2 cell = int2_single(zox_tst_grid_cell);
            for (int i = 0; i < zox_tst_grid_cells; i++) {
                spawn_uic(world, prefab_frame, body, float2_half, int2_zero, cell, cell, color_white, color_black);
            }
            zox_add(body, Refresh);
            break;
        }
        case 1:
            zox_tst_waiting = "the window's handles and its grid";
            handle = zox_tst_window_resize_handle(world, window, (float2) { 1, 0.5f }, &count);
            if (!handle || zox_get_children_count(world, body) < zox_tst_grid_cells) {
                return 0;
            }
            zox_tst_grid_inside(world, canvas, window, body, "spawned");
            before = zox_getv(window, LayoutSize);
            grid_before = zox_getv(body, GridSize);
            zox_setv(handle, DraggingDelta, ((int2) { -2 * zox_tst_grid_cell, 0 }));    // two columns narrower
            zox_setv(handle, DraggableState, 1);
            break;
        case 2:
            zox_tst_waiting = "the narrow window's grid";
            zox_setv(handle, DraggableState, 0);
            zox_setv(handle, DraggingDelta, int2_zero);
            if (zox_getv(body, GridSize).x >= grid_before.x) {
                return 0;
            }
            break;
        case 3: {
            // a frame on: grid_system places the cells (Update)
            const byte2 grid = zox_getv(body, GridSize);
            zox_tst_check(grid.y > grid_before.y && zox_getv(window, LayoutSize).y > before.y,
                "narrowed: the grid is [%ix%i] and the window [%i] high, not more rows and taller than [%ix%i] and [%i]",
                grid.x, grid.y, zox_getv(window, LayoutSize).y, grid_before.x, grid_before.y, before.y);
            zox_tst_grid_inside(world, canvas, window, body, "narrowed");
            grid_before = grid;
            zox_setv(handle, DraggingDelta, ((int2) { 2000, 0 }));     // past the canvas
            zox_setv(handle, DraggableState, 1);
            break;
        }
        case 4:
            zox_tst_waiting = "the wide window's grid";
            zox_setv(handle, DraggableState, 0);
            zox_setv(handle, DraggingDelta, int2_zero);
            if (zox_getv(body, GridSize).x <= grid_before.x) {
                return 0;
            }
            break;
        case 5:
            zox_tst_check(zox_getv(window, LayoutSize).x == zox_getv(canvas, LayoutSize).x,
                "widened past the canvas: the window is [%i] wide, its canvas [%i]", zox_getv(window, LayoutSize).x,
                zox_getv(canvas, LayoutSize).x);
            zox_tst_grid_inside(world, canvas, window, body, "widened");
            return 1;
    }
    step++;
    return 0;
}
