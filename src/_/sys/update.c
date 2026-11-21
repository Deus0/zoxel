
// Refresh becomes Update, which lasts the frame after (systems in any phase see it once)
void refresh_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Refresh);
        zox_add(e, Update);
    }
    zox_sys_on_end();
} zoxd_system(refresh_system);

// the Dirty event: every observer of it has seen it (their commands are deferred), this removes it
void dirty_cleanup_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_remove(it->entities[i], Dirty);
    }
}

// Update lasts one frame, unless the entity was refreshed again (filtered by !Refresh, see core/sys/_.c)
void update_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Update);
    }
    zox_sys_on_end();
} zoxd_system(update_system);
