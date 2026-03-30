entity spawn_window_inspector(ecs *world, entity canvas, entity player, entity target) {

    const char* header_label = "Inspector";
    int visible_count = 12;
    int header_font_size = 22;
    int list_font_size = 12;
    int min_width = 560;

    entity e = spawn_window_list(world, prefab_inspector, player, header_label, header_font_size, NULL, 0, visible_count, list_font_size, (ClickEvent) { NULL }, 1, zox_window_inspector, min_width, zox_alignment_left, byte2_single(2)).x;
    zox_name("inspector");

    // zox_set(e, WindowType, { zox_window_inspector });
    zox_set(e, ElementFontSize, { list_font_size });
    zox_set(e, EntityTarget, { target });

    return e;
}
