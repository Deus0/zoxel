// todo: get dimensions off app using AppLink from Canvas, check it's dimensions

void set_layout_dirty(
    ecs* world,
    entity e
) {
    if (!zox_valid(e)) return;

    if (zox_has(e, LayoutPositionDirty)) {
        zox_muter(e, LayoutPositionDirty, dirty);
        dirty->value = zox_dirty_trigger;
    }

    if (zox_has(e, LayoutSizeDirty)) {
        zox_muter(e, LayoutSizeDirty, dirty);
        dirty->value = zox_dirty_trigger;
    }

    if (zox_has(e, Children)) {
        zox_geter(e, Children, children);
        for (int i = 0; i < children->length; i++) {
            const entity child = children->value[i];
            set_layout_dirty(world, child);
        }
    }
}

void CanvasResizeSystem(iter *it) {

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
        zox_sys_i(AppLink, appLink);
        zox_sys_i(Children, children);
        zox_sys_o(LayoutPosition, position);
        zox_sys_o(LayoutSize, size);
        zox_sys_o(LayoutPositionDirty, position_dirty);
        zox_sys_o(LayoutSizeDirty, size_dirty);

        if (!zox_valid(appLink->value)) {
            continue;
        }

        zox_geter_value(appLink->value, WindowSize, int2, screen_size);
        const int2 pixel_size = screen_to_canvas_size(screen_size, ratio->value);
        if (int2_equals(pixel_size, size->value)) {
            continue;
        }

        size->value = pixel_size;
        position->value = int2_half(pixel_size);
        size_dirty->value = zox_dirty_trigger;
        position_dirty->value = zox_dirty_trigger;

        for (int j = 0; j < children->length; j++) {
            const entity child = children->value[j];
            set_layout_dirty(world, child);
        }

        /*zox_log("Canvas resized [%ix%i] screen [%ix%i]",
            pixel_size.x, pixel_size.y,
            screen_size.x, screen_size.y);*/

    }
} zoxd_system(CanvasResizeSystem)
