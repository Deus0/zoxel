entity spawn_inspector(ecs *world, entity canvas, entity player, entity target) {

    const char* header_label = "Inspector";
    int visible_count = 10;
    int header_font_size = 26;
    int fsize = 14;
    int min_width = 560;

    entity e = spawn_window_list(world, prefab_inspector, player, header_label, header_font_size, NULL, 0, visible_count, fsize, (ClickEvent) { NULL }, 1, zox_window_inspector, min_width, zox_alignment_left, byte2_single(2)).x;
    zox_name("inspector");

    // zox_set(e, WindowType, { zox_window_inspector });
    zox_set(e, FontSize, { fsize });
    zox_set(e, EntityTarget, { target });

    return e;
}
