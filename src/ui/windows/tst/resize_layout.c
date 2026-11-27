// tst: window_resize_layout, every kind of window resized from each side and corner keeps its insides fitting, checked
//  where they're drawn (pointer space bounds, after the frames a resize takes to show)
//  - windows: a game menu's list with rows (spawn_window_list_on), an editor's list (spawn_window_list_at), one with a
//      footer (spawn_window_footer)
//  - each is dragged by a table of handles and deltas: smaller, bigger, to the minimum, past the canvas
//  - after each: nothing drawn outside its window (but its resize handles), the header across its top, the body under
//      it to the footer or bottom, the scroll view on the body, the header's buttons in it, the 8 handles just outside
//      its edges, the window on its canvas
//  - the edges away from the dragged handle stay where they were (but past the canvas): growing to fit its rows spread
//      round its centre, both sides moved
//  - the tests before only checked a window's own size: a grid's body never resized (window_grid_resize)
//  - antitests, what it found: the handles a few pixels out from the edge (offset by their whole thickness, an anchor
//      centres them on it), an empty list's scrollbar handle left at its old height (scrollbar_handle_event skipped
//      it), a game menu narrower than its rows (they spilled left, window_rows_fit)
//  - run: bash bsh/tst.sh window_resize_layout
#define zox_tst_rl_windows 3
#define zox_tst_rl_settle 6     // frames for a resize to reach Position2

typedef struct {
    float2 anchor;
    int2 delta;
} zox_tst_rl_drag;

static const zox_tst_rl_drag zox_tst_rl_drags[] = {
    { { 0, 0 }, { 40, 30 } },       // bottom left, smaller
    { { 1, 1 }, { 60, 50 } },       // top right, bigger
    { { 0, 0.5f }, { 30, 0 } },     // left, narrower
    { { 0.5f, 0 }, { 0, -40 } },    // bottom, taller
    { { 1, 0 }, { -1000, 1000 } },  // bottom right, to the minimum
    { { 0, 1 }, { -2000, 2000 } },  // top left, past the canvas
};

static byte zox_tst_rl_within(int4 inner, int4 outer) {
    return inner.x >= outer.x - 1 && inner.y <= outer.y + 1 && inner.z >= outer.z - 1 && inner.w <= outer.w + 1;
}

static byte zox_tst_rl_bounds(ecs* world, const CanvasViewport* viewport, entity e, int4* bounds) {
    if (!zox_valid(e) || !zox_has(e, Position2) || !zox_has(e, LayoutSize)) {
        return 0;
    }
    *bounds = get_element_pointer_bounds(world, viewport, e, zox_getv(e, Position2), zox_getv(e, LayoutSize));
    return 1;
}

// counts the drawn descendants of e outside bounds (its resize handles aside), naming the first
static int zox_tst_rl_outside(ecs* world, const CanvasViewport* viewport, entity e, int4 bounds, entity* first) {
    int outside = 0;
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity child = it.entities[i];
            if (zox_has(child, WindowResizeHandle) || (zox_has(child, RenderDisabled) && zox_getv(child, RenderDisabled))) {
                continue;
            }
            int4 b;
            // a text's box holds its padding: its glyphs are what's drawn
            if (!zox_has(child, TextData) && zox_tst_rl_bounds(world, viewport, child, &b) && b.y > b.x &&
                !zox_tst_rl_within(b, bounds)) {
                if (!outside && first) {
                    *first = child;
                }
                outside++;
            }
            outside += zox_tst_rl_outside(world, viewport, child, bounds, outside ? NULL : first);
        }
    }
    return outside;
}

// the checks after a drag
static void zox_tst_rl_check(ecs* world, entity canvas, entity window, const char* kind, int drag) {
    CanvasViewport viewport;
    int4 w, b;
    if (!get_canvas_viewport(world, canvas, &viewport) || !zox_tst_rl_bounds(world, &viewport, window, &w)) {
        zox_tst_check(0, "[%s] drag [%i]: no bounds", kind, drag);
        return;
    }
    // on its canvas
    const int4 canvas_bounds = { viewport.position.x, viewport.position.x + viewport.size.x,
        viewport.position.y, viewport.position.y + viewport.size.y };
    zox_tst_check(zox_tst_rl_within(w, canvas_bounds), "[%s] drag [%i]: the window [%i %i %i %i] is off its canvas",
        kind, drag, w.x, w.y, w.z, w.w);
    // nothing drawn outside it
    entity first = 0;
    const int outside = zox_tst_rl_outside(world, &viewport, window, w, &first);
    int4 f = { 0 };
    zox_tst_rl_bounds(world, &viewport, first, &f);
    zox_tst_check(!outside, "[%s] drag [%i]: [%i] drawn outside the window [%i %i %i %i], the first [%s] at [%i %i %i %i]",
        kind, drag, outside, w.x, w.y, w.z, w.w, first ? zox_getn(first) : "", f.x, f.y, f.z, f.w);
    // the header across its top, its buttons in it
    const entity header = zox_get_child_by_id(world, window, zox_id(Header));
    int4 h;
    if (zox_tst_rl_bounds(world, &viewport, header, &h)) {
        zox_tst_check(abs(h.x - w.x) <= 1 && abs(h.y - w.y) <= 1 && abs(h.w - w.w) <= 1,
            "[%s] drag [%i]: the header [%i %i %i %i] isn't across the window's top [%i %i %i %i]",
            kind, drag, h.x, h.y, h.z, h.w, w.x, w.y, w.z, w.w);
        first = 0;
        const int buttons = zox_tst_rl_outside(world, &viewport, header, h, &first);
        zox_tst_check(!buttons, "[%s] drag [%i]: [%i] of the header's parts outside it, the first [%s]", kind, drag,
            buttons, first ? zox_getn(first) : "");
    }
    // the body under the header, to the footer or the bottom
    const entity body = zox_get_child_by_id(world, window, zox_id(WindowBody));
    const entity footer = zox_get_child_by_id(world, window, zox_id(WindowFooter));
    int4 o;
    const int bottom = zox_tst_rl_bounds(world, &viewport, footer, &o) ? o.w : w.z;
    if (zox_tst_rl_bounds(world, &viewport, body, &b)) {
        zox_tst_check(abs(b.x - w.x) <= 1 && abs(b.y - w.y) <= 1 && abs(b.w - h.z) <= 1 && abs(b.z - bottom) <= 1,
            "[%s] drag [%i]: the body [%i %i %i %i] isn't between the header [%i] and [%i], [%i] to [%i] wide",
            kind, drag, b.x, b.y, b.z, b.w, h.z, bottom, w.x, w.y);
    }
    if (footer) {
        zox_tst_check(abs(o.x - w.x) <= 1 && abs(o.y - w.y) <= 1 && abs(o.z - w.z) <= 1,
            "[%s] drag [%i]: the footer [%i %i %i %i] isn't across the window's bottom", kind, drag, o.x, o.y, o.z, o.w);
    }
    // the scroll view on the body
    const entity scrollview = zox_get_link(world, window, ScrollviewLink);
    int4 s;
    if (zox_tst_rl_bounds(world, &viewport, scrollview, &s)) {
        zox_tst_check(abs(s.x - b.x) <= 1 && abs(s.y - b.y) <= 1 && abs(s.z - b.z) <= 1 && abs(s.w - b.w) <= 1,
            "[%s] drag [%i]: the scroll view [%i %i %i %i] isn't the body [%i %i %i %i]", kind, drag,
            s.x, s.y, s.z, s.w, b.x, b.y, b.z, b.w);
    }
    // its 8 handles just outside its edges
    int handles = 0, off = 0;
    iter it = zox_children(world, window);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity handle = it.entities[i];
            int4 r;
            if (!zox_has(handle, WindowResizeHandle) || !zox_tst_rl_bounds(world, &viewport, handle, &r)) {
                continue;
            }
            handles++;
            const float2 a = zox_getv(handle, Anchor);
            const byte wrong = (a.x == 0 && abs(r.y - w.x) > 1) || (a.x == 1 && abs(r.x - w.y) > 1) ||
                (a.y == 0 && abs(r.w - w.z) > 1) || (a.y == 1 && abs(r.z - w.w) > 1);
            if (wrong && !off) {
                zox_log("[tst] [%s] drag [%i]: the handle at [%.1fx%.1f] is [%i %i %i %i], the window [%i %i %i %i]", kind,
                    drag, a.x, a.y, r.x, r.y, r.z, r.w, w.x, w.y, w.z, w.w);
            }
            off += wrong;
        }
    }
    zox_tst_check(handles == 8 && !off, "[%s] drag [%i]: [%i] handles, [%i] not just outside their edge", kind, drag,
        handles, off);
}

// the windows, one at a time (they'd overlap)
static entity zox_tst_rl_spawn(ecs* world, entity canvas, int which) {
    const ClickEvent close = { button_event_close_window };
    if (which == 0) {
        SpawnListElement rows[] = {
            { .text = "Resume", .type = list_element_type_button },
            { .text = "Options", .type = list_element_type_button },
            { .text = "Controls", .type = list_element_type_button },
            { .text = "Exit", .type = list_element_type_button },
            { .text = "Credits", .type = list_element_type_button },
        };
        return spawn_window_list_on(world, prefab_window, canvas, "Menu", 12, 12, close, 1, 0, 160,
            zox_alignment_centre, float2_half, byte2_single(4), NULL, rows, 5, 3).x;
    } else if (which == 1) {
        return spawn_window_list_at(world, prefab_window, canvas, int2_zero, (int2) { 200, 160 }, zox_alignment_left,
            float2_half, "Editor", 12, 12, byte2_single(2), 6, 1, close).x;
    }
    entity footer;
    return spawn_window_footer(world, prefab_window, prefab_body, "Footer", canvas, int2_zero, (int2) { 200, 120 },
        float2_half, 12, byte2_single(4), (void*) button_event_close_window, 24, &footer).x;
}

// a window's edges: left, right, bottom, top
static int4 zox_tst_rl_edges(ecs* world, entity window) {
    const int2 p = zox_getv(window, LayoutPosition), s = zox_getv(window, LayoutSize);
    return (int4) { p.x - s.x / 2, p.x + s.x / 2, p.y - s.y / 2, p.y + s.y / 2 };
}

// the edges the drag's handle isn't on stayed (a side handle keeps 3 of them, a corner 2)
static void zox_tst_rl_kept(ecs* world, entity window, int4 before, float2 anchor, const char* kind, int drag) {
    const int4 after = zox_tst_rl_edges(world, window);
    const byte left = anchor.x != 0, right = anchor.x != 1, bottom = anchor.y != 0, top = anchor.y != 1;
    const byte kept = (!left || abs(after.x - before.x) <= 1) && (!right || abs(after.y - before.y) <= 1) &&
        (!bottom || abs(after.z - before.z) <= 1) && (!top || abs(after.w - before.w) <= 1);
    zox_tst_check(kept, "[%s] drag [%i]: edges [%i %i %i %i] became [%i %i %i %i], the far side moved", kind, drag,
        before.x, before.y, before.z, before.w, after.x, after.y, after.z, after.w);
}

static byte zox_tst_window_resize_layout_ready(ecs* world) {
    static int4 edges;
    static int step, frames, drag, which;
    static entity canvas, window, handle;
    static const char* const kinds[] = { "menu list", "editor list", "footer" };
    const int drags = sizeof(zox_tst_rl_drags) / sizeof(zox_tst_rl_drag);
    int count;
    zox_tst_progress(step * 100 + which * 10 + drag);
    frames++;
    switch (step) {
        case 0:
            zox_tst_waiting = "the fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            window = zox_tst_rl_spawn(world, canvas, which);
            break;
        case 1:
            zox_tst_waiting = "the window to open and lay out";
            if (frames < zox_tst_rl_settle || zox_getv(window, LocalScale1) != 1) {
                return 0;
            }
            zox_tst_rl_check(world, canvas, window, kinds[which], -1);
            break;
        case 2:
            handle = zox_tst_window_resize_handle(world, window, zox_tst_rl_drags[drag].anchor, &count);
            zox_tst_check(handle, "[%s]: no handle for drag [%i]", kinds[which], drag);
            if (!handle) {
                return 1;
            }
            edges = zox_tst_rl_edges(world, window);
            zox_setv(handle, DraggingDelta, zox_tst_rl_drags[drag].delta);
            zox_setv(handle, DraggableState, 1);
            break;
        case 3:
            zox_setv(handle, DraggableState, 0);
            zox_setv(handle, DraggingDelta, int2_zero);
            break;
        case 4:
            zox_tst_waiting = "the resize to show";
            if (frames < zox_tst_rl_settle) {
                return 0;
            }
            zox_tst_rl_check(world, canvas, window, kinds[which], drag);
            if (drag != drags - 1) {    // the last drags past the canvas, it keeps the window on it
                zox_tst_rl_kept(world, window, edges, zox_tst_rl_drags[drag].anchor, kinds[which], drag);
            }
            frames = 0;
            if (++drag < drags) {
                step = 2;
                return 0;
            }
            // the next window
            drag = 0;
            zox_delete(window);
            if (++which >= zox_tst_rl_windows) {
                return 1;
            }
            window = zox_tst_rl_spawn(world, canvas, which);
            step = 1;
            return 0;
    }
    step++;
    frames = 0;
    return 0;
}
