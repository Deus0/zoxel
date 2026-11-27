// tst: window_resize, a floating window's side and corner handles resize it, the opposite edge stays; the mouse
//  hovers one just outside the window's edge (it lights up) and drags it
//  (ins/resize.c, sys/layout.c, elements2/sys/drag.c)
static entity zox_tst_window_resize_handle(ecs* world, entity window, float2 anchor, int* count) {
    entity found = 0;
    *count = 0;
    iter it = zox_children(world, window);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity handle = it.entities[i];
            if (zox_has(handle, WindowResizeHandle)) {
                (*count)++;
                if (float2_equals(zox_getv(handle, Anchor), anchor)) {
                    found = handle;
                }
            }
        }
    }
    return found;
}

// each drag: the handle's anchor, its delta, then the window's size and position after it
typedef struct {
    float2 anchor;
    int2 delta;
    int2 size;
    int2 position;
} zox_tst_window_resize_drag;

static const zox_tst_window_resize_drag zox_tst_window_resize_drags[] = {
    { { 1, 0.5f }, { 40, 0 }, { 260, 160 }, { 20, 0 } },       // right side
    { { 0, 0.5f }, { -30, 0 }, { 290, 160 }, { 5, 0 } },       // left side
    { { 1, 1 }, { 10, 20 }, { 300, 180 }, { 10, 10 } },        // top right corner
    { { 1, 0.5f }, { -500, 0 }, { 120, 180 }, { -80, 10 } },   // right side, past the minimum width
    { { 0.5f, 0 }, { 0, -20 }, { 120, 200 }, { -80, 0 } },     // bottom side
    { { 0.5f, 1 }, { 0, -40 }, { 120, 160 }, { -80, -20 } },   // top side
    { { 0, 0 }, { -30, 10 }, { 150, 150 }, { -95, -15 } },     // bottom left corner
    { { 0, 1 }, { 10, 10 }, { 140, 160 }, { -90, -10 } },      // top left corner
    { { 1, 0 }, { 20, -20 }, { 160, 180 }, { -80, -20 } },     // bottom right corner
    { { 0.5f, 1 }, { 0, -500 }, { 160, 80 }, { -80, -70 } },   // top side, past the minimum height
};

// the mouse drags the right side handle 40 to the right: hover (it lights up), press, move, release
//  - returns 1 when done, 0 to step on, and waits by returning 2
static byte zox_tst_window_resize_mouse_step(ecs* world, entity canvas, entity window, entity* handle, int2* before,
    int2* point, int phase)
{
    int count;
    switch (phase) {
        case 0:
            *handle = zox_tst_window_resize_handle(world, window, (float2) { 1, 0.5f }, &count);
            if (!zox_tst_ui_pointer_position(world, canvas, *handle, point)) {
                return 2;
            }
            zox_tst_mouse(*point, 0);
            return 0;
        case 1:
            zox_tst_waiting = "the mouse to hover the right side handle";
            if (zox_tst_mouse_target(world, dbg_player) != *handle) {
                return 2;
            }
            zox_tst_waiting = "the hovered resize handle to light up";
            if (!color_equals(zox_getv(*handle, FillColor), zox_getv(*handle, SelectedFillColor))) {
                return 2;
            }
            *before = zox_getv(window, LayoutSize);
            zox_tst_mouse(*point, 1);
            return 0;
        case 2:
            zox_tst_waiting = "the right side handle to start dragging";
            if (!zox_getv(*handle, DraggableState)) {
                return 2;
            }
            zox_tst_mouse(int2_add(*point, (int2) { 40, 0 }), 1);
            return 0;
        case 3:
            zox_tst_waiting = "the mouse drag to resize the window";
            if (zox_getv(window, LayoutSize).x == before->x) {
                return 2;
            }
            zox_tst_mouse(int2_add(*point, (int2) { 40, 0 }), 0);
            zox_tst_check(zox_getv(window, LayoutSize).x > before->x, "the mouse drag didn't widen the window");
            return 1;
    }
    return 1;
}

static byte zox_tst_window_resize_ready(ecs* world) {
    static int step;
    static entity canvas, window, handle;
    static int2 before, point;
    const int drags = sizeof(zox_tst_window_resize_drags) / sizeof(zox_tst_window_resize_drag);
    int count;
    zox_tst_progress(step);
    if (step == 0) {
        zox_tst_waiting = "the fixture canvas";
        canvas = zox_tst_ui_fixture_canvas(world);
        if (!canvas) {
            return 0;
        }
        window = spawn_window_list_at(world, prefab_window, canvas, int2_zero, (int2) { 220, 160 },
            zox_alignment_left, float2_half, "Resize", 18, 12, byte2_single(2), 6, 1,
            (ClickEvent) { button_event_close_window }).x;
        step++;
        return 0;
    }
    if (step == 1) {
        zox_tst_waiting = "the window's 8 resize handles";
        zox_tst_window_resize_handle(world, window, float2_zero, &count);
        if (count < 8) {
            return 0;
        }
        zox_tst_check(count == 8, "window has [%i] resize handles, not 8", count);
        step++;
    }
    // even steps start a drag, odd ones wait for it and check
    const int drag = (step - 2) / 2;
    if (drag >= drags) {
        const byte done = zox_tst_window_resize_mouse_step(world, canvas, window, &handle, &before, &point,
            step - 2 - drags * 2);
        step += !done;
        return done == 1;
    }
    const zox_tst_window_resize_drag d = zox_tst_window_resize_drags[drag];
    if (step % 2 == 0) {
        handle = zox_tst_window_resize_handle(world, window, d.anchor, &count);
        zox_tst_check(handle, "no resize handle at [%.1fx%.1f]", d.anchor.x, d.anchor.y);
        if (!handle) {
            return 1;
        }
        before = zox_getv(window, LayoutSize);
        zox_setv(handle, DraggingDelta, d.delta);
        zox_setv(handle, DraggableState, 1);
        step++;
        return 0;
    }
    zox_tst_waiting = "the resize drag";
    if (int2_equals(zox_getv(window, LayoutSize), before)) {
        return 0;
    }
    zox_setv(handle, DraggableState, 0);
    zox_setv(handle, DraggingDelta, int2_zero);
    const int2 size = zox_getv(window, LayoutSize);
    const int2 position = zox_getv(window, LayoutPosition);
    zox_tst_check(int2_equals(size, d.size) && int2_equals(position, d.position),
        "drag [%i]: window is [%ix%i] at [%ix%i], not [%ix%i] at [%ix%i]", drag,
        size.x, size.y, position.x, position.y, d.size.x, d.size.y, d.position.x, d.position.y);
    step++;
    return 0;
}
