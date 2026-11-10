/*zox_sys2(ItemIconLabelSystem) {
    byte dbg_log = 0;
    byte label_text_capacity = 8;
    zox_sys_world();
    zox_sys_begin();
    // zox_sys_in(SlotLink);
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // zox_sys_i(SlotLink, slot);
        zox_sys_o(TextData, text);
        entity slot = zox_get_link(world, e, SlotLink);
        if (!zox_valid(slot) ||
            !zox_is_linked_any(world, slot->value, DataLink))
        {
            continue;
        }
        entity e2 = zox_get_link(world, slot->value, DataLink);
        if (!zox_valid(e2) || !zox_has(e2, Item)) {
            continue;
        }
        byte quantity = zox_has(e2, Quantity) ?
            zox_getv(e2, Quantity) :
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
                zox_log("Item Frame Label [%s] Set to [%s]",
                    zox_get_name(e),
                    result);
            }
        }
    }
} zox_sys_end(ItemIconLabelSystem);
*/
