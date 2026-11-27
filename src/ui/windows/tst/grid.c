// tst: window_grid_resize, a grid window (an inventory's slots) resized: its columns fit its width, its rows follow and
//  scroll in its body, it stays on its canvas (sys/layout.c window_grid_fit, window_canvas_fit)
//  - a grid window (ins/grid.c: its body a scroll view, the grid its list, a vertical bar while its rows don't fit)
//  - a window of 16 cells, 4 by 4, made narrow by its right handle: fewer columns, more rows, the same height, the
//      rows that don't fit past its body (its Mask clips them); the fake mouse's wheel over it scrolls a row down
//  - its bottom right corner dragged to the minimum (a window's, 120 x 80): one row shows; then wide past the canvas: as wide as the canvas,
//      every row shown, its scrollbar hidden, the window on it
//  - before: the window grew taller to fit its rows, round its centre, so it moved about as they reflowed (play test
//      2026-11-27); before that its cells spilled out of a narrow window
//  - run: bash bsh/tst.sh window_grid_resize
#define zox_tst_grid_cells 16
#define zox_tst_grid_cell 40

// its scroll view's vertical bar shows
static byte zox_tst_grid_bar_shown(ecs* world, entity body) {
    const entity bar = scrollview_vertical_bar(world, zox_get_link(world, body, ScrollviewLink));
    return bar && !zox_getv(bar, RenderDisabled);
}

// the shown rows' cells inside its body, the scrolled away ones past it, and the window inside its canvas
static void zox_tst_grid_inside(ecs* world, entity canvas, entity window, entity body, const char* when) {
    const int2 body_size = zox_getv(body, LayoutSize);
    const byte2 grid = zox_getv(body, GridSize);
    const int start = zox_getv(body, ListStart), visible = zox_getv(body, ListVisible);
    int wrong = 0, count = 0;
    iter it = zox_children(world, body);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const int2 p = zox_getv(it.entities[i], LayoutPosition);
            const int2 s = zox_getv(it.entities[i], LayoutSize);
            const byte inside = abs(p.x) + s.x / 2 <= body_size.x / 2 && abs(p.y) + s.y / 2 <= body_size.y / 2;
            const int row = count / int_max(1, grid.x);
            const byte shown = row >= start && row < start + visible;
            wrong += inside != shown;
            count++;
        }
    }
    zox_tst_check(!wrong && grid.x * grid.y >= count, "%s: [%i] of [%i] cells wrongly in or out of the [%ix%i] body, grid "
        "[%ix%i], rows [%i] from [%i]", when, wrong, count, body_size.x, body_size.y, grid.x, grid.y, visible, start);
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
            const entity3 e3 = spawn_window_grid(world, prefab_window, canvas, "Grid", int2_zero, (int2) { side, side },
                float2_half, 12, byte2_single(4), NULL);
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
            if (!handle || zox_get_children_count(world, body) < zox_tst_grid_cells || !zox_has(body, ListVisible)) {
                return 0;
            }
            zox_tst_grid_inside(world, canvas, window, body, "spawned");
            zox_tst_check(!zox_tst_grid_bar_shown(world, body), "spawned: every row fits, its scrollbar shows");
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
            zox_tst_check(grid.y > grid_before.y && zox_getv(window, LayoutSize).y == before.y &&
                zox_getv(body, ListVisible) < grid.y,
                "narrowed: the grid is [%ix%i], [%i] rows shown, the window [%i] high, not more rows than [%ix%i] scrolling "
                "in [%i]", grid.x, grid.y, zox_getv(body, ListVisible), zox_getv(window, LayoutSize).y, grid_before.x,
                grid_before.y, before.y);
            zox_tst_grid_inside(world, canvas, window, body, "narrowed");
            zox_tst_check(zox_tst_grid_bar_shown(world, body), "narrowed: its rows don't fit, its scrollbar is hidden");
            grid_before = grid;
            // the fake mouse over its body, its wheel down a notch
            int2 point;
            if (!zox_tst_ui_pointer_position(world, canvas, body, &point)) {
                return 0;
            }
            zox_tst_mouse(point, 0);
            zox_tst_mouse_wheel(-1);
            break;
        }
        case 4:
            zox_tst_waiting = "the wheel to scroll its rows";
            zox_tst_mouse_wheel(0);
            if (zox_getv(body, ListStart) != 1) {
                return 0;
            }
            break;
        case 5:
            zox_tst_mouse_end();
            zox_tst_grid_inside(world, canvas, window, body, "scrolled");
            handle = zox_tst_window_resize_handle(world, window, (float2) { 1, 0 }, &count);   // bottom right
            zox_setv(handle, DraggingDelta, ((int2) { -2000, 2000 }));     // to the minimum
            zox_setv(handle, DraggableState, 1);
            break;
        case 6: {
            zox_tst_waiting = "the smallest window's grid";
            zox_setv(handle, DraggableState, 0);
            zox_setv(handle, DraggingDelta, int2_zero);
            if (zox_getv(body, ListVisible) != 1) {
                return 0;
            }
            break;
        }
        case 7:
            zox_tst_grid_inside(world, canvas, window, body, "smallest");
            zox_tst_check(zox_getv(body, LayoutSize).y < 2 * zox_tst_grid_cell, "the smallest body is [%i] high, more than "
                "a row", zox_getv(body, LayoutSize).y);
            grid_before = zox_getv(body, GridSize);
            handle = zox_tst_window_resize_handle(world, window, (float2) { 1, 0.5f }, &count);
            zox_setv(handle, DraggingDelta, ((int2) { 2000, 0 }));     // past the canvas
            zox_setv(handle, DraggableState, 1);
            break;
        case 8:
            zox_tst_waiting = "the wide window's grid";
            zox_setv(handle, DraggableState, 0);
            zox_setv(handle, DraggingDelta, int2_zero);
            if (zox_getv(body, GridSize).x <= grid_before.x) {
                return 0;
            }
            break;
        case 9:
            zox_tst_check(zox_getv(window, LayoutSize).x == zox_getv(canvas, LayoutSize).x,
                "widened past the canvas: the window is [%i] wide, its canvas [%i]", zox_getv(window, LayoutSize).x,
                zox_getv(canvas, LayoutSize).x);
            zox_tst_grid_inside(world, canvas, window, body, "widened");
            zox_tst_check(zox_getv(body, ListVisible) < zox_getv(body, GridSize).y || !zox_tst_grid_bar_shown(world, body),
                "widened: every row fits, its scrollbar still shows");
            return 1;
    }
    step++;
    return 0;
}
