// if outside list visible indexes, hide, otherwise show
// This sets visibility of the list recursively
zox_sys2(ListRenderDirtySystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ListPositionDirty);
    zox_sys_in(ListStart);
    zox_sys_in(ListVisible);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ListPositionDirty, state);
        zox_sys_i(ListStart, start);
        zox_sys_i(ListVisible, visible);
        if (state->value != zox_dirty_active) {
            continue;
        }
        uint k = 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (!zox_valid(e2)) {
                    continue;
                }
                byte is_visible = k >= start->value && k < start->value + visible->value;
                set_children_render_disabled(world, e2, !is_visible);
                k++;
            }
        }
    }
} zox_sys_end(ListRenderDirtySystem);
