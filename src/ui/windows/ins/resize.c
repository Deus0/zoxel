// a floating window's 8 resize handles around its edges, invisible until hovered: 4 side strips, 4 corner squares (drag.c's DragResizes resizes it)
static const float2 window_resize_anchors[8] = {
    { 0, 0.5f }, { 1, 0.5f }, { 0.5f, 0 }, { 0.5f, 1 },     // left, right, bottom, top
    { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 },                 // corners
};

// a handle's size for a window size: sides run along an edge, corners are squares
static int2 window_resize_handle_size(int2 window_size, float2 anchor) {
    const int thick = 6;
    const byte side_x = anchor.x == 0 || anchor.x == 1;
    const byte side_y = anchor.y == 0 || anchor.y == 1;
    if (side_x && side_y) {
        return (int2) { thick, thick };
    }
    return side_x ? (int2) { thick, window_size.y } : (int2) { window_size.x, thick };
}

// just outside the window's edge, so it never covers the window's own buttons: an anchor puts its centre on the edge,
//  half its thickness out puts its side on it (no gap to miss)
static int2 window_resize_handle_position(float2 anchor) {
    const int half = 3;
    return (int2) { anchor.x == 0 ? -half : anchor.x == 1 ? half : 0, anchor.y == 0 ? -half : anchor.y == 1 ? half : 0 };
}

static void spawn_window_resize_handles(ecs* world, entity window, int2 size) {
    for (int i = 0; i < 8; i++) {
        const float2 anchor = window_resize_anchors[i];
        const entity handle = spawn_handle(world, prefab_handle, window, size, window_resize_handle_position(anchor),
            window_resize_handle_size(size, anchor), anchor, (color) { 0, 0, 0, 0 }, (color) { 0, 0, 0, 0 }, 0);
        zox_add(handle, DragResizes);
        zox_add(handle, WindowResizeHandle);
        zox_link(world, handle, DraggedLink, window);
        zox_setv(handle, LayoutConstraints, int4_zero);
        zox_setv(handle, BonusLayer, 6);
        zox_setv(handle, SelectedFillColor, ((color) { 255, 255, 255, 60 }));   // shows while hovered
    }
}