// todo: get dimensions off app using AppLink from Canvas, check it's dimensions

void set_layout_dirty_recursive(ecs* world, entity e) {
    if (!zox_valid(e)) {
        return;
    }

    if (zox_has(e, LayoutPositionDirty)) {
        zox_muter(e, LayoutPositionDirty, dirty);
        dirty->value = zox_dirty_trigger;
        zox_log("+++ Position Dirty [%s] +++", zox_get_name(e));
    }

    if (zox_has(e, LayoutSizeDirty)) {
        zox_muter(e, LayoutSizeDirty, dirty);
        dirty->value = zox_dirty_trigger;
    }

    if (zox_has(e, Children)) {
        zox_geter(e, Children, children);
        for (int i = 0; i < children->length; i++) {
            const entity child = children->value[i];
            set_layout_dirty_recursive(world, child);
        }
    }
}


// ATM this just checks every frame if WindowSize changed
// TODO: Link Camera to Canvas (CanvasLinks) and use LayoutSizeDirty on Camera / LayoutSize
zox_sys2(CanvasResizeSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ScreenToCanvas);
    zox_sys_in(AppLink);
    zox_sys_in(Children);
    zox_sys_out(LayoutPosition);
    zox_sys_out(LayoutSize);
    zox_sys_out(LayoutPositionDirty);
    zox_sys_out(LayoutSizeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ScreenToCanvas, ratio);
        zox_sys_i(AppLink, app);
        zox_sys_i(Children, children);
        zox_sys_o(LayoutPosition, position);
        zox_sys_o(LayoutPositionDirty, pdirty);
        zox_sys_o(LayoutSize, size);
        zox_sys_o(LayoutSizeDirty, sdirty);

        if (!zox_valid(app->value)) {
            continue;
        }

        zox_geter_value(app->value, WindowSize, int2, screen_size);
        int2 viewport_size = screen_to_canvas_size(screen_size, ratio->value);
        if (int2_equals(viewport_size, size->value)) {
            continue;
        }

        size->value = viewport_size;
        position->value = int2_half(viewport_size);
        sdirty->value = zox_dirty_trigger;
        pdirty->value = zox_dirty_trigger;

        for (int j = 0; j < children->length; j++) {
            entity child = children->value[j];
            set_layout_dirty_recursive(world, child);
        }

        zox_log("Canvas resized [%ix%i] screen [%ix%i]", size->value.x, size->value.y, screen_size.x, screen_size.y);

    }
} zox_sys_end(CanvasResizeSystem);
