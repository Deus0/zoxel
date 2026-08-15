// todo: get dimensions off app using AppLink from Canvas, check it's dimensions
void set_layout_dirty_recursive(ecs* world, entity e) {
    if (!zox_valid(e)) {
        return;
    }
    if (zox_has(e, LayoutPositionDirty)) {
        zox_setm(e, LayoutPositionDirty, zox_dirty_trigger);
    }
    if (zox_has(e, LayoutSizeDirty)) {
        zox_setm(e, LayoutSizeDirty, zox_dirty_trigger);
    }
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            entity e2 = it.entities[i];
            set_layout_dirty_recursive(world, e2);
        }
    }
}

// NOTE: this just checks every frame if WindowSize changed
// TODO: Link Camera to Canvas (CanvasLinks) and use LayoutSizeDirty on Camera / LayoutSize
zox_sys2(CanvasResizeSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ScreenToCanvas);
    zox_sys_in(AppLink);
    zox_sys_out(LayoutPosition);
    zox_sys_out(LayoutSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ScreenToCanvas, ratio);
        zox_sys_i(AppLink, app);
        zox_sys_o(LayoutPosition, position);
        zox_sys_o(LayoutSize, size);
        if (!zox_valid(app->value)) {
            continue;
        }
        int2 screen_size = zox_getv(app->value, WindowSize);
        int2 viewport_size = screen_to_canvas_size(screen_size, ratio->value);
        if (int2_equals(viewport_size, size->value)) {
            continue;
        }
        size->value = viewport_size;
        position->value = int2_half(viewport_size);
        //sdirty->value = zox_dirty_trigger;
        //pdirty->value = zox_dirty_trigger;
        if (dbg_log) {
            zox_log("Canvas has Resized [%ix%i]", viewport_size.x, viewport_size.y);
        }
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                set_layout_dirty_recursive(world, e2);
            }
        }
        // zox_log("Canvas resized [%ix%i] screen [%ix%i]", size->value.x, size->value.y, screen_size.x, screen_size.y);
    }
} zox_sys_end(CanvasResizeSystem);
