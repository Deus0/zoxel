static void dropdown_set_selected(ecs* world, entity dropdown, entity item) {
    zox_set_link(world, dropdown, DropdownSelectedLink, item);
    zox_add(dropdown, Dirty);
}

static void dropdown_pick(ecs* world, ClickEventData event) {
    if (zox_has(event.clicked, ItemListDisabled) || zox_getv(event.clicked, RenderDisabled)) {
        return;
    }
    const entity popup = zox_get_parent(world, event.clicked);
    const entity dropdown = zox_get_parent(world, popup);
    dropdown_set_selected(world, dropdown, event.clicked);
    item_list_close(world, dropdown);
}

entity spawn_dropdown(ecs* world, entity parent, entity canvas, int2 position, int2 size, float2 anchor,
    const char* const* options, byte count, byte selected, byte font_size, byte layer)
{
    count = int_min(count, int_min(layouts2_children_capacity, 255));
    selected = selected < count ? selected : 0;
    const entity button = spawn_item_list_button(world, prefab_dropdown, parent, count ? options[selected] : "",
        position, size, anchor, font_size, layer, zox_alignment_centre).x;
    zox_setv(button, ClickEvent, item_list_toggle);
    zox_set_link(world, button, ItemListCanvas, canvas);
    const entity arrow = spawn_text(world, prefab_text, button, (int2) { -font_size / 2 - 2, 0 },
        (float2) { 1, 0.5f }, font_size, zox_alignment_centre, byte2_single(0), "v",
        button_font_fill, button_font_outline);
    if (!ui_is3D(world, parent)) {
        zox_setv(arrow, Layer, layer + 1);
    }
    const entity popup = spawn_item_list_popup(world, button, (int2) { size.x, count * size.y });
    for (uint i = 0; i < count; i++) {
        const entity item = spawn_item_list_item(world, popup, options[i], size, i, font_size, (ClickEvent) { dropdown_pick });
        if (i == selected) {
            zox_set_link(world, button, DropdownSelectedLink, item);
        }
    }
    return button;
}
