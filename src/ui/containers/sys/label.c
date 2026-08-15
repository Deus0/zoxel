/*zox_sys2(DataFrameLabelSystem) {
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
        entity e2 = zox_getv(slot->value, DataLink);
        if (zox_valid(e2)) {
            continue;
        }
        char result[1];
        result[0] = '\0';
        // set text of quantity label
        if (!is_zext(text, result)) {
            set_zext(text, result);
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(DataFrameLabelSystem);
*/