
void layout_position_dirty_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, LayoutPositionDirty);
        zox_add(e, LayoutPositionUpdate);
    }
} zoxd_system(layout_position_dirty_system);

void layout_position_update_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, LayoutPositionUpdate);
    }
} zoxd_system(layout_position_update_system);
