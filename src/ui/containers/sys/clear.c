// NOTE: Clears text when data removed
zox_sys2(IconLabelClearSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    //zox_sys_in(DataDirty);
    zox_sys_in(SlotLink);
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
       // zox_sys_i(DataDirty, state);
        zox_sys_i(SlotLink, slot);
        zox_sys_o(TextData, text);
        if (!zox_valid(slot->value) ||
            !zox_has(slot->value, DataLink))
        {
            continue;
        }
        /*byte dirty2 = zox_getv(slot->value, DataDirty) == zox_dirty_active;
        dirty2 |= state->value == zox_dirty_active;
        if (!dirty2) {
            continue;
        }*/
        entity data = zox_getv(slot->value, DataLink);
        if (zox_valid(data)) {
            continue;
        }
        if (!text->length) {
            continue;
        }
        resize_TextData(text, 0);
        zox_add(e, Dirty);
        if (dbg_log) {
            zox_log("Clearing Frame Label [%s]", zox_get_name(e));
        }
    }
} zox_sys_end(IconLabelClearSystem);
