// The hierarchy and inspector windows: a third of the canvas wide, full height, on one side,
// a scrolling list with the editor's colours
//  - side: -1 left, 1 right
entity4 spawn_editor_window(
    ecs* world,
    entity prefab,
    entity canvas,
    const char* header_label,
    int side,
    int font_size)
{
    const int2 canvas_size = zox_getv(canvas, LayoutSize);
    const int2 size = (int2) { canvas_size.x / 3, canvas_size.y };
    const int2 position = (int2) { side * (canvas_size.x / 2 - size.x / 2), 0 };
    const int header_font_size = 18;
    const byte scrollbar_width = 6;
    entity4 e4 = spawn_window_list_at(
        world,
        prefab,
        canvas,
        position,
        size,
        zox_alignment_left,
        float2_half,
        header_label,
        header_font_size,
        font_size,
        byte2_single(2),
        scrollbar_width,
        1,
        (ClickEvent) { NULL });
    const entity header = e4.y;
    const entity body = e4.z;
    zox_setv(body, FillColor, editor_window_fill);
    zox_setv(body, OutlineColor, editor_window_outline);
    zox_setv(header, FillColor, editor_header_fill);
    zox_setv(header, OutlineColor, editor_header_outline);
    zox_setv(header, ElementFillColor, editor_header_fill);
    zox_setv(header, ElementOutlineColor, editor_header_outline);
    return e4;
}
