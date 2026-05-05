// active links
zox_sys2(ElementActiveLinkSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ActiveState);
    zox_sys_in(ActiveStateDirty);
    //zox_sys_in(ParentLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ActiveState, state);
        zox_sys_i(ActiveStateDirty, dirty);
        //zox_sys_i(ParentLink, parent);
        entity parent = zox_get_parent(world, e);

        if (dirty->value != zox_dirty_active || !state->value) {
            continue;
        }

        if (!zox_valid(parent) || !zox_has(parent, ActiveLink)) {
            continue;
        }

        zox_mut_begin(parent, ActiveLink, active);

        if (active->value == e) {
            continue;
        }

        // resets last list active
        if (zox_valid(active->value) && zox_has(parent, ActiveSingle)) {
            zox_set(active->value, ActiveState, { 0 });
            zox_set(active->value, ActiveStateDirty, { zox_dirty_trigger });
        }

        // zox_log("Active Old %s -> New %s", active->value ? zox_get_name(active->value) : "None", zox_get_name(e));

        // Link our active to our list
        active->value = e;
        zox_modified(parent, ActiveLink);
    }
} zox_sys_end(ElementActiveLinkSystem);
