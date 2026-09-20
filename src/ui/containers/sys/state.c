
void data_dirty_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, DataDirty);
        zox_add(e, DataUpdate);
    }
} zoxd_system(data_dirty_system);

void data_update_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, DataUpdate);
    }
} zoxd_system(data_update_system);

// slot dirty to ui
void slot_ui_update_system(iter* it) {
    byte dbg_log = 1;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity slot = zox_getv(e, SlotLink);
        if (zox_valid(slot) && zox_has(slot, DataUpdate)) {
            zox_add(e, DataDirty);
            if (dbg_log) {
                entity data = zox_getv(slot, DataLink);
                zox_log("Slot was Dirty, updating UI [%s], Data [%s]",
                    zox_getn(e),
                    zox_getn(data));
            }
        }
    }
} zoxd_system(slot_ui_update_system);


// slot dirty to ui
void data_ui_update_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity data = zox_getv(e, DataLink);
        if (zox_valid(data) && zox_has(data, DataUpdate)) {
            zox_add(e, DataDirty);
        }
    }
} zoxd_system(data_ui_update_system);

