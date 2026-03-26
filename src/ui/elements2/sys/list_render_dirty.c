// if outside list visible indexes, hide, otherwise show
zox_sys2(ListRenderDirtySystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ListPositionDirty);
    zox_sys_in(ListStart);
    zox_sys_in(ListVisible);
    zox_sys_in(Children);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ListPositionDirty, state);
        zox_sys_i(ListStart, start);
        zox_sys_i(ListVisible, visible);
        zox_sys_i(Children, children);

        if (state->value != zox_dirty_active) {
            continue;
        }

        for (int j = 0; j < children->length; j++) {
            entity e2 = children->value[j];

            if (!zox_valid(e2)) {
                continue;
            }

            byte is_visible = j >= start->value && j <  start->value + visible->value;
            // zox_set(e2, RenderDisabled, { !is_visible });
            set_children_render_disabled(world, e2, !is_visible);
        }
    }
} zox_sys_end(ListRenderDirtySystem);
