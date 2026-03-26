// TODO: Refactor:
//      - spawn scrollview
//      - hierarchy dirty just resets list and doesn't touch scrollbar etc'
//      - list dirty will refresh / move scrolling buttons

// why does raycasting mess up after this spawns
entity spawn_window_hierarchy(ecs *world, entity canvas, entity player, entity root) {

    int visible_count = 10;
    const char* header_label = "hierarchy";
    int list_font_size = 16;
    int header_font_size = list_font_size * 2;

    SpawnListElement elements[1];
    elements[0] = (SpawnListElement) {
        .text = "--- --- Loading --- ---",
        .on_click = { NULL },
    };

    // prefab_menu_game
    entity e = spawn_window_list(world, prefab_hierarchy, player, "hierarchy", header_font_size, elements, 1, visible_count, list_font_size, (ClickEvent) { NULL }, 1, zox_window_hierarchy);
    zox_name("hierarchy");

    zox_set(e, ElementFontSize, { list_font_size });
    // zox_set(e, WindowType, { zox_window_hierarchy });
    zox_set(e, EditorTarget, { root });

    hierarchy = e;
    return e;
}
