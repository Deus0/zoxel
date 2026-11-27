static entity item_list_popup(ecs* world, entity list) {
    return zox_get_child_by_id(world, list, zox_id(ItemListPopup));
}

static entity item_list_item_at(ecs* world, entity list, uint index) {
    const entity popup = item_list_popup(world, list);
    if (!popup) {
        return 0;
    }
    iter children = zox_children(world, popup);
    while (zox_children_next(children)) {
        for (int i = 0; i < children.count; i++) {
            const entity item = children.entities[i];
            if (zox_has(item, ItemListItem) && zox_getv(item, ChildIndex) == index) {
                zox_children_end(children);
                return item;
            }
        }
    }
    return 0;
}

static byte item_list_is_open(ecs* world, entity list) {
    const entity popup = item_list_popup(world, list);
    return popup && !zox_getv(popup, RenderDisabled);
}

static void item_list_close(ecs* world, entity list) {
    const entity popup = item_list_popup(world, list);
    if (popup) {
        set_children_render_disabled(world, popup, 1);
    }
    const entity canvas = zox_get_link(world, list, ItemListCanvas);
    if (canvas && zox_get_link(world, canvas, OpenItemList) == list) {
        zox_set_link(world, canvas, OpenItemList, 0);
    }
}

static void item_list_open(ecs* world, entity list) {
    const entity popup = item_list_popup(world, list);
    if (!popup || !zox_getv(popup, LayoutSize).y || zox_getv(list, RenderDisabled)) {
        return;
    }
    const entity canvas = zox_get_link(world, list, ItemListCanvas);
    const entity previous = zox_get_link(world, canvas, OpenItemList);
    if (previous && previous != list) {
        const entity previous_popup = item_list_popup(world, previous);
        if (previous_popup) {
            set_children_render_disabled(world, previous_popup, 1);
        }
    }
    set_children_render_disabled(world, popup, 0);
    zox_set_link(world, canvas, OpenItemList, list);
}

static void item_list_toggle(ecs* world, ClickEventData event) {
    if (item_list_is_open(world, event.clicked)) {
        item_list_close(world, event.clicked);
    } else {
        item_list_open(world, event.clicked);
    }
}

static entity spawn_item_list_popup(ecs* world, entity list, int2 size) {
    // under its button, left edges lined up (desktop menus): anchored at the button's bottom left
    const entity popup = spawn_ui(world, prefab_layout2, list, (float2) { 0, 0 },
        (int2) { size.x / 2, -size.y / 2 }, size, size);
    zox_add(popup, ItemListPopup);
    zox_setv(popup, RenderDisabled, 1);
    if (!ui_is3D(world, list)) {
        zox_setv(popup, Layer, 240);
    }
    return popup;
}

static entity2 spawn_item_list_button(ecs* world, entity prefab, entity parent, const char* label,
    int2 position, int2 size, float2 anchor, byte font, byte layer, byte alignment)
{
    const entity2 button = spawn_button(world, prefab, parent, label, position, size, anchor,
        alignment, font, byte2_single(alignment == zox_alignment_left ? 4 : 0), button_fill, button_outline, button_font_fill, button_font_outline);
    zox_add(button.y, TextLabel);
    if (alignment == zox_alignment_left) {
        zox_setv(button.y, Anchor, float2_left);    // its text from the button's left edge, not its centre
        zox_setv(button.y, LayoutPosition, ((int2) { 4, 0 }));
    }
    if (!ui_is3D(world, parent)) {
        zox_setv(button.x, Layer, layer);
        zox_setv(button.y, Layer, layer + 1);
    }
    return button;
}

static entity spawn_item_list_item(ecs* world, entity parent, const char* label, int2 size,
    uint index, byte font, ClickEvent click)
{
    const entity2 button = spawn_item_list_button(world, prefab_clickable, parent, label,
        (int2) { 0, -(int) index * size.y - size.y / 2 }, size, (float2) { 0.5f, 1 }, font, 241, zox_alignment_left);
    zox_add(button.x, ItemListItem);
    zox_setv(button.x, ChildIndex, index);
    zox_setv(button.x, ClickEvent, click.value);
    zox_setv(button.x, RenderDisabled, 1);
    zox_setv(button.y, RenderDisabled, 1);
    return button.x;
}

void item_list_set_disabled(ecs* world, entity item, byte disabled) {
    if (disabled) {
        zox_add(item, ItemListDisabled);
    } else {
        zox_remove(item, ItemListDisabled);
    }
    zox_add(item, Dirty);
}

void item_list_set_checked(ecs* world, entity item, byte checked) {
    zox_setv(item, ItemListChecked, checked);
    zox_add(item, Dirty);
}
