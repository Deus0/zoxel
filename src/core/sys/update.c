
void dirty_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Dirty);
        zox_add(e, Update);
    }
} zoxd_system(dirty_system);

// Update lasts one frame, unless the entity was dirtied again (dirty_system re-adds it)
//  - both run deferred in zoxp_remove: without this check, a Dirty on the frame after another
//      queued [remove Dirty, add Update, remove Update], losing the second update
//      (i.e. a tooltip cleared then set on back to back frames kept its old glyphs, hidden)
void update_system(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        if (zox_has(e, Dirty)) {
            continue;
        }
        zox_remove(e, Update);
    }
} zoxd_system(update_system);
