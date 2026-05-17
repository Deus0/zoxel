zox_sys2(ItemIconLabelSystem) {
    byte label_text_capacity = 8;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SlotLink);
    zox_sys_out(TextData);
    zox_sys_out(TextDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(SlotLink, slot);
        zox_sys_o(TextData, text);
        zox_sys_o(TextDirty, dirty);
        if (!zox_valid(slot->value) || !zox_has(slot->value, DataLink)) {
            continue;
        }
        entity e2 = zox_gett_value(slot->value, DataLink);
        if (!zox_valid(e2) || !zox_has(e2, Item) || !zox_has(e2, Quantity) || !zox_has(e2, QuantityDirty)) {
            continue;
        }
        zox_geter_value(e2, QuantityDirty, byte, quantity_dirty);
        if (text->length && quantity_dirty != zox_dirty_active) {
            continue;
        }
        zox_geter_value(e2, Quantity, byte, quantity);
        char result[label_text_capacity];
        if (quantity > 1) {
            snprintf(result, label_text_capacity, "x%i", quantity);
        } else {
            result[0] = '\0';
        }
        // set text of quantity label
        if (!is_zext(text, result)) {
            set_zext(text, result);
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(ItemIconLabelSystem);
