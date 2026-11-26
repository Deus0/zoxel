entity spawn_menu(ecs* world, entity canvas, entity parent, const char* name, int2 position, int2 size, float2 anchor)
{
    const entity menu = spawn_item_list_button(world, prefab_button, parent, name, position, size, anchor, 12, 33).x;
    zox_add(menu, Menu);
    zox_setv(menu, ClickEvent, item_list_toggle);
    zox_set_link(world, menu, ItemListCanvas, canvas);
    spawn_item_list_popup(world, menu, (int2) { int_max(size.x, 120), 0 });
    return menu;
}

entity spawn_menu_item(ecs* world, entity menu, const char* label, ClickEvent click) {
    // The popup can still be queued when a whole menu is built in a click.
    const entity item = spawn_item_list_item(world, menu, label, (int2) { 120, 24 }, 0, 12, click);
    const entity tick = spawn_text(world, prefab_text, item, (int2) { -10, 0 },
        (float2) { 1, 0.5f }, 12, zox_alignment_centre, byte2_single(0), "", button_font_fill, button_font_outline);
    zox_add(tick, ItemListTick);
    zox_setv(tick, RenderDisabled, 1);
    if (!ui_is3D(world, menu)) {
        zox_setv(tick, Layer, 242);
    }
    zox_add(tick, Dirty);
    return item;
}
