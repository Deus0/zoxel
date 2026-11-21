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
        // NOTE: labels without slots (stats, data grids) arn't item labels
        if (!zox_valid(slot)) {
            continue;
        }
        entity item = zox_valid(slot) ?
            zox_get_link(world, slot, DataLink) :
            0;
        // NOTE: empty slots / non items still clear the label
        byte quantity =
            zox_valid(item) &&
            zox_has(item, Item) &&
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
            zox_add(e, Refresh);
            if (dbg_log) {
                zox_log("Item Frame Label [%s] Set to [%s] Length [%i]",
                    zox_get_name(e),
                    result,
                    text->length);
            }
        }
    }
}

// an item icon's tooltip text: its item's name and quantity, "" without one
static inline byte item_icon_tooltip_text(ecs* world, entity icon, char* text, uint size) {
    const entity slot = zox_get_link(world, icon, SlotLink);
    const entity item = zox_valid(slot) ? zox_get_link(world, slot, DataLink) : 0;
    // TODO: We should do this for only X types of items (BlockItem, ConsumeableItem, etc)
    if (!zox_valid(item) || !zox_has(item, Item) || !zox_has(item, Quantity)) {
        text[0] = '\0';
        return 0;
    }
    const char* name = zox_has(item, ZoxName) ? zox_getv(item, ZoxName) : zox_getn(item);
    snprintf(text, size, "[%s] x%i\n", name, zox_getv(item, Quantity));
    return 1;
}

static inline entity item_icon_tooltip(ecs* world, entity icon) {
    const entity canvas = zox_get_parent_by_id(world, icon, zox_id(Canvas));
    return zox_valid(canvas) ? zox_get_child_by_id(world, canvas, zox_id(Tooltip)) : 0;
}

// Tooltip Event for item icons: selected, its tooltip shows its item
void item_icon_tooltip_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        char text[128];
        const entity tooltip = item_icon_tooltip(world, e);
        if (zox_valid(tooltip) && item_icon_tooltip_text(world, e, text, sizeof(text))) {
            set_tooltip_text(world, e, tooltip, text);
        }
    }
}

// its item changed while its tooltip shows it (picked up, placed, stacked, swapped): the tooltip follows, empty when
//  it's gone (it kept showing the item picked up, on an empty slot)
void item_icon_tooltip_update_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity tooltip = item_icon_tooltip(world, e);
        if (!zox_valid(tooltip) || zox_get_link(world, tooltip, TooltipLink) != e) {
            continue;
        }
        char text[128];
        item_icon_tooltip_text(world, e, text, sizeof(text));
        set_entity_text(world, tooltip, text);
    }
}
