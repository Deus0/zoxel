
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
