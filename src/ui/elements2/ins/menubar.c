entity spawn_menubar(ecs* world, entity canvas, entity parent, int2 size, int2 position, float2 anchor) {
    const entity bar = spawn_uic(world, prefab_frame, parent, anchor, position, size, size, frame_fill, frame_outline);
    zox_add(bar, Menubar);
    zox_add_ordered_children(bar);
    zox_set_link(world, bar, ItemListCanvas, canvas);
    if (!ui_is3D(world, parent)) {
        zox_setv(bar, Layer, 32);
    }
    return bar;
}

entity spawn_menubar_menu(ecs* world, entity menubar, const char* name) {
    const int width = int_max(48, 12 * (int) strlen(name) + 12);
    const entity menu = spawn_menu(world, zox_get_link(world, menubar, ItemListCanvas), menubar, name,
        int2_zero, (int2) { width, 24 }, (float2) { 0, 0.5f });
    zox_add(menu, Dirty);
    return menu;
}
