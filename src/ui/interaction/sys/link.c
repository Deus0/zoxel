// active links
void ElementActiveLinkSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ActiveState);
    zox_sys_in(ActiveStateDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ActiveState, state);
        zox_sys_i(ActiveStateDirty, dirty);
        entity parent = zox_get_parent(world, e);
        if (dirty->value != zox_dirty_active || !state->value) {
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
            zox_set(active, ActiveStateDirty, { zox_dirty_trigger });
        }
        // Link our active to our list
        zox_set_link(world, parent, ActiveLink, e);
    }
    zox_sys_on_end();
} zoxd_system(ElementActiveLinkSystem);
