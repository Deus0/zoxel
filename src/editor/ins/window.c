entity4 spawn_editor_window(
    ecs* world,
    entity prefab,
    entity canvas,
    const char* header_label,
    float2 anchor,
    int2 size,
    int font_size)
{
    ensure_editor_menubar(world, canvas);
    const int header_font_size = 18;
    const byte scrollbar_width = 6;
    entity4 e4 = spawn_window_list_at(
        world,
        prefab,
        canvas,
        int2_zero,
        size,
        zox_alignment_left,
        anchor,
        header_label,
        header_font_size,
        font_size,
        byte2_single(2),
        scrollbar_width,
        1,
        (ClickEvent) { button_event_close_window });
    const entity header = e4.y;
    const entity body = e4.z;
    zox_setv(body, FillColor, editor_window_fill);
    zox_setv(body, OutlineColor, editor_window_outline);
    zox_setv(header, FillColor, editor_header_fill);
    zox_setv(header, OutlineColor, editor_header_outline);
    zox_setv(header, ElementFillColor, editor_header_fill);
    zox_setv(header, ElementOutlineColor, editor_header_outline);
    zox_add(e4.x, Docked);
    // tighter rows than a game menu's: more fit in the editor's windows
    const entity list = e4.w;
    zox_setv(list, TextPadding, ((byte2) { 3 * ui_scale, 1 * ui_scale }));
    zox_setv(list, ListMargins, ((byte2) { 4 * ui_scale, 2 * ui_scale }));
    zox_setv(list, ListPadding, ((byte2) { 0, 1 * ui_scale }));
    spawn_window_search_button(world, e4.x, e4.y);
    zox_add(e4.x, EditorThemeDirty);
    return e4;
}
