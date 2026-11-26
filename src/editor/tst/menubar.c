static void zox_tst_editor_menu_item_click(ecs* world, entity item) {
    zox_defer_begin();
    on_element_clicked(world, dbg_player, item);
    zox_defer_end();
}

static byte zox_tst_editor_menubar_ready(ecs* world) {
    static int step;
    zox_tst_progress(step);
    if (!step) {
        if (!zox_tst_ui_fixture_canvas(world)) {
            return 0;
        }
        toggle_hierarchy(world, zox_key_y);
        step++;
        return 0;
    }

    const entity editor_canvas = get_editor_canvas(world);
    const entity4 docks = docks_find(world, editor_canvas);
    const entity bar = docks.w;
    const ordered_children menus = zox_get_ordered_children(world, bar);
    if (!bar || menus.length != 4) {
        zox_tst_waiting = "the four docked editor menus";
        return 0;
    }
    const entity window = menus.value[2];
    const entity hierarchy = item_list_item_at(world, window, 0);
    const entity tick = zox_get_child_by_id(world, hierarchy, zox_id(ItemListTick));
    const entity dock = zox_get_child_by_id(world, editor_canvas, zox_id(HierarchyUI));
    if (step == 1) {
        const int2 canvas_size = zox_getv(editor_canvas, LayoutSize);
        const int2 bar_size = zox_getv(bar, LayoutSize);
        const int2 bar_position = zox_getv(bar, LayoutPosition);
        const int2 dock_size = zox_getv(dock, LayoutSize);
        const int2 dock_position = zox_getv(dock, LayoutPosition);
        zox_tst_check(zox_has(bar, Menubar) && zox_has(bar, EditorElement) && bar_size.x == canvas_size.x &&
            bar_size.y == 24 && bar_position.x == 0 && bar_position.y + bar_size.y / 2 == 0,
            "menubar did not fill the top dock");
        zox_tst_check(dock && dock == docks.x && dock_position.y + dock_size.y / 2 == canvas_size.y / 2 - bar_size.y,
            "hierarchy did not start below the menubar");
        zox_tst_click(world, dbg_player, window);
        zox_tst_check(zox_getv(hierarchy, ItemListChecked) && is_zext(zox_get(tick, TextData), "v"),
            "opening the hierarchy by key did not tick Window > Hierarchy");
        zox_tst_editor_menu_item_click(world, hierarchy);
    } else if (step == 2) {
        zox_tst_check(!dock && !zox_getv(hierarchy, ItemListChecked), "Hierarchy item did not close and untick the window");
        zox_tst_click(world, dbg_player, window);
        zox_tst_editor_menu_item_click(world, hierarchy);
    } else {
        zox_tst_check(dock && zox_getv(hierarchy, ItemListChecked), "Hierarchy item did not open and tick the window");
        const entity close = zox_get_child_by_id_recursive(world, dock, zox_id(CloseButton));
        zox_tst_click(world, dbg_player, close);
        zox_tst_click(world, dbg_player, window);
        zox_tst_check(!zox_valid(dock) && !zox_getv(hierarchy, ItemListChecked) && is_zext(zox_get(tick, TextData), ""),
            "the hierarchy close button left its item ticked");
        const entity edit = menus.value[1];
        zox_tst_click(world, dbg_player, edit);
        const entity undo = item_list_item_at(world, edit, 0);
        const int descendants = zox_tst_count_descendants(world, editor_canvas);
        zox_tst_editor_menu_item_click(world, undo);
        zox_tst_check(zox_has(undo, ItemListDisabled) && zox_getv(undo, ClickDisabled) &&
            item_list_is_open(world, edit) && zox_tst_count_descendants(world, editor_canvas) == descendants,
            "disabled Undo reacted to a click");
        return 1;
    }
    step++;
    return 0;
}
