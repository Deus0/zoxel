static void item_list_pointer_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity pointer = it->entities[i];
        const entity device = zox_get_link(world, pointer, DeviceLink);
        const entity player = zox_get_parent(world, device);
        const entity target = zox_getv(pointer, Target);
        zox_each_link(world, player, CanvasLink, canvas) {
            const entity list = zox_get_link(world, canvas, OpenItemList);
            if (!list) {
                continue;
            }
            entity hit = target;
            while (hit && hit != list) {
                hit = zox_get_parent(world, hit);
            }
            if (!hit) {
                item_list_close(world, list);
            }
        }
    }
}

static void item_list_update_item(ecs* world, entity item) {
    const byte disabled = zox_has(item, ItemListDisabled);
    const entity label = zox_get_child_by_id(world, item, zox_id(TextLabel));
    const entity tick = zox_get_child_by_id(world, item, zox_id(ItemListTick));
    const color fill = disabled ? color_gray : button_font_fill;
    zox_setm(item, ClickDisabled, disabled);
    set_entity_text_fill(world, label, fill);
    if (tick) {
        set_entity_text_fill(world, tick, fill);
        set_entity_text(world, tick, zox_has(item, ItemListChecked) && zox_getv(item, ItemListChecked) ? "v" : "");
    }
}

static void item_list_item_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        item_list_update_item(world, it->entities[i]);
    }
}

static void menu_item_layout_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity item = zox_get_parent(world, it->entities[i]);
        const entity menu = zox_get_parent(world, item);
        const entity popup = item_list_popup(world, menu);
        const int height = zox_getv(menu, LayoutSize).y;
        int2 size = zox_getv(popup, LayoutSize);
        zox_set_parent_non_fragment(world, item, popup);
        zox_setv(item, ChildIndex, size.y / height);
        zox_setv(item, LayoutSize, ((int2) { size.x, height }));
        zox_setv(item, LayoutPosition, ((int2) { 0, -size.y - height / 2 }));
        size.y += height;
        zox_setm(popup, LayoutSize, size);
        zox_setm(popup, LayoutPosition, ((int2) { 0, -size.y / 2 }));
        set_children_render_disabled(world, item, zox_getv(popup, RenderDisabled));
        item_list_update_item(world, item);
    }
}

static void dropdown_selected_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity dropdown = it->entities[i];
        const entity item = zox_get_link(world, dropdown, DropdownSelectedLink);
        const entity label = item ? zox_get_child_by_id(world, item, zox_id(TextLabel)) : 0;
        char* text = label ? get_zext_text(zox_get(label, TextData)) : NULL;
        set_entity_text(world, zox_get_child_by_id(world, dropdown, zox_id(TextLabel)), text ? text : "");
        free(text);
    }
}

static void menu_item_click_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity item = it->entities[i];
        const entity menu = zox_get_parent(world, zox_get_parent(world, item));
        if (!zox_has(menu, Menu) || zox_has(item, ItemListDisabled) || zox_getv(item, RenderDisabled)) {
            continue;
        }
        if (zox_has(item, ItemListChecked)) {
            item_list_set_checked(world, item, !zox_getv(item, ItemListChecked));
        }
        item_list_close(world, menu);
    }
}
