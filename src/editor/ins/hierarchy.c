// TODO: Refactor:
//      - spawn scrollview
//      - hierarchy dirty just resets list and doesn't touch scrollbar etc'
//      - list dirty will refresh / move scrolling buttons

// why does raycasting mess up after this spawns
entity spawn_window_hierarchy(ecs *world, entity canvas, entity player, entity root) {

    const char* header_label = "Hierarchy";
    int visible_count = 10;
    int header_font_size = 26;
    int fsize = 14;
    int min_width = 440;

    // prefab_menu_game
    entity3 e = spawn_window_list(world, prefab_hierarchy, player, header_label, header_font_size, NULL, 0, visible_count, fsize, (ClickEvent) { NULL }, 1, zox_window_hierarchy, min_width, zox_alignment_left, byte2_single(2), NULL);
    zox_set_name_spawned(world, e.x, "hierarchy");

    zox_set(e.x, ElementFontSize, { fsize });
    zox_set(e.x, EntityTarget, { root });

    zox_add_tag(e.y, ActiveSingle);
    zox_set(e.y, ActiveLink, { 0 });

    return e.x;
}
