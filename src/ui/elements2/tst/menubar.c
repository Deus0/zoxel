static byte zox_tst_menubar_ready(ecs* world) {
    static entity canvas, file, edit;
    static byte step;
    zox_tst_progress(step);
    if (!step) {
        canvas = zox_tst_ui_fixture_canvas(world);
        if (!canvas) {
            return 0;
        }
        zox_defer_begin();
        const entity bar = spawn_menubar(world, canvas, canvas, (int2) { 240, 24 }, int2_zero, float2_half);
        file = spawn_menubar_menu(world, bar, "File");
        spawn_menu_item(world, file, "Open", (ClickEvent) { 0 });
        edit = spawn_menubar_menu(world, bar, "Edit");
        spawn_menu_item(world, edit, "Undo", (ClickEvent) { 0 });
        zox_defer_end();
        zox_tst_check(zox_getv(bar, LayoutSize).x == 240 &&
            zox_getv(edit, LayoutPosition).x > zox_getv(file, LayoutPosition).x, "menubar layout is wrong");
        zox_tst_click(world, dbg_player, file);
        zox_tst_check(item_list_is_open(world, file), "first menu didn't open");
        zox_tst_click(world, dbg_player, edit);
        zox_tst_check(!item_list_is_open(world, file) && item_list_is_open(world, edit), "second menu didn't replace the first");
        zox_tst_check(zox_get_link(world, canvas, OpenItemList) == edit, "canvas doesn't link the second menu");
        step++;
        return 0;
    }
    if (step == 1) {
        int2 position;
        if (!zox_tst_ui_pointer_position(world, canvas, file, &position)) {
            return 0;
        }
        zox_tst_mouse(position, 0);
        step++;
        return 0;
    }
    if (zox_tst_mouse_target(world, dbg_player) != file) {
        zox_tst_waiting = "the mouse to hover the File menu";
        return 0;
    }
    zox_tst_check(!item_list_is_open(world, edit) && item_list_is_open(world, file), "hover didn't switch menus");
    return 1;
}
