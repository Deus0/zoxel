zox_sys2(WindowElementClickedSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ClickState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ClickState, state);
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity window = zox_get_parent_by_id(world, e, zox_id(Window));
        if (!zox_valid(window)) {
            continue;
        }
        zox_set(canvas, WindowToTop, { window });
    }
} zox_sys_end(WindowElementClickedSystem);
