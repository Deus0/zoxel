// Tooltip Event for item icons
void icon_label_event(iter* it) {
    byte dbg_log = 0;
    byte label_text_capacity = 8;
    zox_sys_world();
    zox_sys_begin();
    //zox_sys_in(SlotLink);
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        //zox_sys_i(SlotLink, slot);
        zox_sys_o(TextData, text);
        entity slot = zox_get_link(world, e, SlotLink);
        if (!zox_valid(slot) ||
            !zox_has(slot, DataLink))
        {
            continue;
        }
        entity data = zox_getv(slot, DataLink);
        if (!zox_valid(data) || !zox_has(data, Item)) {
            continue;
        }
        byte quantity =
            zox_has(data, Quantity) ?
                zox_getv(data, Quantity) :
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
}
