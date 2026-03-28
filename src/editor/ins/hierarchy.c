// TODO: Refactor:
//      - spawn scrollview
//      - hierarchy dirty just resets list and doesn't touch scrollbar etc'
//      - list dirty will refresh / move scrolling buttons

// why does raycasting mess up after this spawns
entity spawn_window_hierarchy(ecs *world, entity canvas, entity player, entity root) {

    const char* header_label = "Hierarchy";
    int visible_count = 10;
    int header_font_size = 18;
    int list_font_size = 16;
    int min_width = 500;

    // prefab_menu_game
    entity e = spawn_window_list(world, prefab_hierarchy, player, header_label, header_font_size, NULL, 0, visible_count, list_font_size, (ClickEvent) { NULL }, 1, zox_window_hierarchy, min_width);
    zox_name("hierarchy");

    zox_set(e, ElementFontSize, { list_font_size });
    // zox_set(e, WindowType, { zox_window_hierarchy });
    zox_set(e, EntityTarget, { root });

    return e;
}
