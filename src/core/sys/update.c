
void dirty_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Dirty);
        zox_add(e, Update);
    }
} zoxd_system(dirty_system);

void update_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Update);
    }
} zoxd_system(update_system);
