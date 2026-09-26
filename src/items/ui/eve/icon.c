// Tooltip Event for item icons
void item_icon_label_event(iter* it) {
    byte dbg_log = 0;
    byte label_text_capacity = 8;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TextData, text);
        entity slot = zox_get_link(world, e, SlotLink);
        if (!zox_valid(slot) ||
            !zox_has(slot, DataLink))
        {
            continue;
        }
        entity item = zox_getv(slot, DataLink);
        if (!zox_valid(item) ||
            !zox_has(item, Item))
        {
            continue;
        }
        byte quantity =
            zox_has(item, Quantity) ?
                zox_getv(item, Quantity) :
                0;
        char result[label_text_capacity];
        if (quantity > 1) {
            snprintf(result, label_text_capacity, "x%i", quantity);
        } else {
            result[0] = '\0';
        }
        // set text of quantity label
        if (!is_zext(text, result)) {
            set_zext(text, result);
            zox_add(e, Dirty);
            if (dbg_log) {
                zox_log("Item Frame Label [%s] Set to [%s] Length [%i]",
                    zox_get_name(e),
                    result,
                    text->length);
            }
        }
    }
}

// Tooltip Event for item icons
void item_icon_tooltip_event(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity slot = zox_get_link(world, e, SlotLink);
        if (!zox_valid(slot) ||
            !zox_has(slot, DataLink))
        {
            continue;
        }
        // zox_log("icon_tooltip_event [%s]", zox_sys_e_name);
        entity item = zox_getv(slot, DataLink);
        // TODO: We should do this for only X types of items
        //      - BlockItem, ConsumeableItem, etc
        if (!zox_valid(item) ||
            !zox_has(item, Item) ||
            !zox_has(item, Quantity))
        {
            continue;
        }
        entity canvas = zox_get_parent_by_id(
            world,
            e,
            zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity tooltip = zox_get_child_by_id(
            world,
            canvas,
            zox_id(Tooltip));
        if (!tooltip) {
            zox_loge("Tooltip not found in canvas");
            continue;
        }
        const char* name =
            zox_has(item, ZoxName) ?
                zox_getv(item, ZoxName) :
                zox_getn(item);
        byte quantity =
            zox_has(item, Quantity) ?
                zox_getv(item, Quantity) :
                1;
        char result[128];
        sprintf(
            result,
            "[%s] x%i\n",
            name,
            quantity);
        set_tooltip_text(
            world,
            e,
            tooltip,
            result);
        if (dbg_log) {
            zox_log("Item Icon Tooltip: [%s]: [%s]",
                zox_sys_e_name,
                result);
        }
    }
}
