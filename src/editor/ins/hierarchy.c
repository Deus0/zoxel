// TODO: Refactor:
//      - spawn scrollview
//      - hierarchy dirty just resets list and doesn't touch scrollbar etc'
//      - list dirty will refresh / move scrolling buttons

// why does raycasting mess up after this spawns
entity spawn_window_hierarchy(ecs *world, entity canvas, entity player, entity root) {
    int2 canvas_size = zox_getv(canvas, LayoutSize);
    int2 size = (int2) { canvas_size.x / 3, canvas_size.y };
    int2 position = (int2) { -canvas_size.x / 2 + size.x / 2, 0 };
    const char* header_label = "Hierarchy";
    int header_font_size = 18;
    int font_size = 12;
    byte scrollbar_width = 6;
    entity4 e4 = spawn_window_list_at(world, prefab_hierarchy, canvas, position, size, zox_alignment_left, header_label, header_font_size, font_size, byte2_single(2), scrollbar_width, 1, (ClickEvent) { NULL });
    entity window = e4.x;
    entity header = e4.y;
    entity body = e4.z;
    entity list = e4.w;
    zox_set_name_spawned(world, window, "hierarchy");
    zox_add_tag(window, HierarchyUI);
    zox_set(window, ElementFontSize, { font_size });
    zox_set(window, EntityTarget, { root });
    // Add Editor Colors
    zox_set(body, FillColor, { editor_window_fill });
    zox_set(body, OutlineColor, { editor_window_outline });
    zox_set(header, FillColor, { editor_header_fill });
    zox_set(header, OutlineColor, { editor_header_outline });
    zox_set(header, ElementFillColor, { editor_header_fill });
    zox_set(header, ElementOutlineColor, { editor_header_outline });
    // Add linker to hierarchy list
    zox_add_tag(list, ActiveSingle);
    zox_set(list, ActiveLink, { 0 });
    return window;
}
