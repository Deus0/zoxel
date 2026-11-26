static int zox_tst_menu_calls;

static void zox_tst_menu_action(ecs* world, ClickEventData event) {
    (void) world;
    (void) event;
    zox_tst_menu_calls++;
}

static void zox_tst_menu_click(ecs* world, entity item) {
    zox_defer_begin();
    on_element_clicked(world, dbg_player, item);
    zox_defer_end();
}

static byte zox_tst_menu_ready(ecs* world) {
    const entity canvas = zox_tst_ui_fixture_canvas(world);
    if (!canvas) {
        return 0;
    }
    zox_defer_begin();
    const entity menu = spawn_menu(world, canvas, canvas, "File", int2_zero, (int2) { 80, 24 }, float2_half);
    const entity first = spawn_menu_item(world, menu, "Open", (ClickEvent) { zox_tst_menu_action });
    const entity disabled = spawn_menu_item(world, menu, "Save", (ClickEvent) { zox_tst_menu_action });
    item_list_set_disabled(world, disabled, 1);
    const entity toggle = spawn_menu_item(world, menu, "Grid", (ClickEvent) { 0 });
    item_list_set_checked(world, toggle, 0);
    zox_defer_end();

    zox_tst_check(item_list_item_at(world, menu, 0) == first &&
        item_list_item_at(world, menu, 1) == disabled && item_list_item_at(world, menu, 2) == toggle,
        "menu items aren't in popup order");
    zox_tst_click(world, dbg_player, menu);
    const entity label = zox_get_child_by_id(world, disabled, zox_id(TextLabel));
    zox_tst_check(color_equal(zox_getv(label, FontFillColor), color_gray), "disabled item wasn't greyed out");
    zox_tst_menu_click(world, disabled);
    zox_tst_check(!zox_tst_menu_calls && item_list_is_open(world, menu), "disabled click had an effect");

    zox_tst_menu_click(world, first);
    zox_tst_check(zox_tst_menu_calls == 1 && !item_list_is_open(world, menu), "action didn't run once and close the menu");

    zox_tst_click(world, dbg_player, menu);
    const entity tick = zox_get_child_by_id(world, toggle, zox_id(ItemListTick));
    zox_tst_check(tick && is_zext(zox_getm(tick, TextData), ""), "unchecked toggle showed a tick");
    zox_tst_menu_click(world, toggle);
    zox_tst_check(!item_list_is_open(world, menu), "toggle click didn't close the menu");
    zox_tst_click(world, dbg_player, menu);
    zox_tst_check(item_list_is_open(world, menu) && zox_getv(toggle, ItemListChecked) && tick &&
        !zox_getv(tick, RenderDisabled) && is_zext(zox_getm(tick, TextData), "v"), "checked toggle didn't show a tick");
    return 1;
}
