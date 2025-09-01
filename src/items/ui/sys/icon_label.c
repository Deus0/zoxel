void ItemIconLabelSystem(iter *it) {

    int label_text_count = 8;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ItemLink);
    zox_sys_out(TextData);
    zox_sys_out(TextDirty);

    for (int i = 0; i < it->count; i++) {

        // zox_sys_e();
        zox_sys_i(ItemLink, item);
        zox_sys_o(TextData, data);
        zox_sys_o(TextDirty, dirty);

        if (!zox_valid(item->value) || !zox_has(item->value, QuantityDirty)) {
            continue;
        }

        zox_geter_value(item->value, QuantityDirty, byte, quantity_dirty);

        if (data->length && quantity_dirty != zox_dirty_active) {
            continue;
        }

        zox_geter_value(item->value, Quantity, byte, quantity);
        char text[label_text_count];


        if (quantity > 1) {
            snprintf(text, label_text_count, "x%i", quantity);
        } else {
            text[0] = '\0';
        }

        // set text of quantity label
        if (!is_zext(data, text)) {
            set_zext(data, text);
            dirty->value = zox_dirty_trigger;
        }
    }
} zoxd_system2(ItemIconLabelSystem);