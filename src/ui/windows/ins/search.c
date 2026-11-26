// a window's search: a button in its header shows a bar under it, a text field and a filter type (sys/search.c)
//  - the bar is spawned once, hidden when closed (its text cleared), so a relayout in the same frame sees it gone
#define window_search_bar_height 24
#define window_search_name 0      // the filter types (window_search_get), in the dropdown order
#define window_search_tag 1
#define window_search_type_width 56
static const char* const window_search_types[] = { "name", "tag" };
static void window_search_toggle(ecs* world, ClickEventData event);

// its search bar while shown, 0 if it has none or it is hidden
static entity window_search_bar_shown(ecs* world, entity window) {
    const entity bar = zox_get_child_by_id(world, window, zox_id(WindowSearchBar));
    return bar && !zox_getv(bar, RenderDisabled) ? bar : 0;
}

// header: the window's header (pass it from the spawn: in the frame a window spawns its header can't be looked up)
entity spawn_window_search_button(ecs* world, entity window, entity header) {
    if (!header) {
        return 0;
    }
    const entity button = spawn_button(world, prefab_clickable, header, "s",
        (int2) { -68, 0 }, (int2) { 20, 20 }, (float2) { 1, 0.5f }, zox_alignment_centre,
        12, byte2_single(0), header_fill, header_outline, header_font_fill, header_font_outline).x;
    zox_setv(button, ClickEvent, window_search_toggle);
    zox_add(button, WindowSearchButton);
    return button;
}

// the bar under the header: an input field and a dropdown of the filter types
static entity spawn_window_search_bar(ecs* world, entity window) {
    const entity canvas = zox_get_parent_by_id(world, window, zox_id(Canvas));
    const entity header = zox_get_child_by_id(world, window, zox_id(Header));
    const int header_height = header ? zox_getv(header, LayoutSize).y : 0;
    const int2 size = { zox_getv(window, LayoutSize).x, window_search_bar_height };
    const byte layer = zox_getv(window, Layer) + 2;
    const entity bar = spawn_uic(world, prefab_element_frame, window, (float2) { 0.5f, 1 },
        (int2) { 0, -header_height - size.y / 2 }, size, size, footer_fill, footer_outline);
    zox_set_unique_name(bar, "search");
    zox_add(bar, WindowSearchBar);
    const int2 field_size = { int_max(20, size.x - window_search_type_width - 6), size.y - 4 };
    const entity field = spawn_button(world, prefab_button, bar, "", (int2) { field_size.x / 2 + 2, 0 },
        field_size, (float2) { 0, 0.5f }, zox_alignment_centre, 12, byte2_single(2),
        button_fill, button_outline, button_font_fill, button_font_outline).x;
    zox_add(field, TextLabel);
    zox_add(field, TextInput);
    zox_setv(field, ClickEvent, &on_click_input_field);
    spawn_dropdown(world, bar, canvas, (int2) { -window_search_type_width / 2 - 2, 0 },
        (int2) { window_search_type_width, size.y - 4 }, (float2) { 1, 0.5f },
        window_search_types, 2, 0, 12, layer);
    return bar;
}
