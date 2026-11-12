
void dirty_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Dirty);
        zox_add(e, Update);
    }
    zox_sys_on_end();
} zoxd_system(dirty_system);

// Update lasts one frame, unless the entity was dirtied again (filtered by !Dirty, see core/sys/_.c)
void update_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Update);
    }
    zox_sys_on_end();
} zoxd_system(update_system);
