// active links
void element_active_link_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ActiveState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ActiveState, state);
        // the last reader of it (element_active_system runs before)
        zox_remove(e, ActiveStateDirty);
        entity parent = zox_get_parent(world, e);
        if (!state->value) {
            continue;
        }
        if (!zox_valid(parent) || !zox_has(parent, ActiveSingle)) {
            continue;
        }
        const entity active = zox_get_link(world, parent, ActiveLink);
        if (active == e) {
            continue;
        }
        // resets last list active
        if (zox_valid(active)) {
            zox_set(active, ActiveState, { 0 });
            zox_add(active, ActiveStateDirty);
        }
        // Link our active to our list
        zox_set_link(world, parent, ActiveLink, e);
    }
    zox_sys_on_end();
} zoxd_system(element_active_link_system);
