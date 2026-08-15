entity spawn_inspector(ecs *world, entity canvas, entity player, entity target) {
    int2 canvas_size = zox_getv(canvas, LayoutSize);
    int2 size = (int2) { canvas_size.x / 3, canvas_size.y };
    int2 position = (int2) { canvas_size.x / 2 - size.x / 2, 0 };
    const char* header_label = "Inspector";
    int header_font_size = 18;
    int font_size = 12;
    byte scrollbar_width = 6;
    entity4 e4 = spawn_window_list_at(world, prefab_inspector, canvas, position, size, zox_alignment_left, float2_half, header_label, header_font_size, font_size, byte2_single(2), scrollbar_width, 1, (ClickEvent) { NULL });
    entity window = e4.x;
    entity header = e4.y;
    entity body = e4.z;
    zox_set_name_spawned(world, window, "inspector");
    zox_add(window, InspectorUI);
    zox_set(window, FontSize, { font_size });
    zox_set(window, EntityTarget, { target });
    // Add Editor Colors
    zox_set(body, FillColor, { editor_window_fill });
    zox_set(body, OutlineColor, { editor_window_outline });
    zox_set(header, FillColor, { editor_header_fill });
    zox_set(header, OutlineColor, { editor_header_outline });
    zox_set(header, ElementFillColor, { editor_header_fill });
    zox_set(header, ElementOutlineColor, { editor_header_outline });
    return window;
}
