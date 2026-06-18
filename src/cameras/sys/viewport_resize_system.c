zox_sys2(ViewportResizeSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(WindowSizeDirty);
    zox_sys_in(WindowSize);
    zox_sys_in(CameraLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(WindowSizeDirty, dirty);
        zox_sys_i(WindowSize, size);
        zox_sys_i(CameraLinks, cameras);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (dbg_log) {
            zox_log("Viewport Dirty [%i] cameras at [%ix%i]", cameras->length, size->value.x, size->value.y);
        }
        // refresh viewport
        for (int j = 0; j < cameras->length; j++) {
            entity camera = cameras->value[j];
            if (!zox_valid(camera)) {
                continue;
            }
            // TODO: Camera should have CanvasLinks and a size dirty flag!
            float4 screen_to_canvas = zox_get_value(camera, ScreenToCanvas);
            int2 new_size = screen_to_canvas_size(size->value, screen_to_canvas);
            int2 position = screen_to_canvas_position(size->value, screen_to_canvas);
            if (zox_has(camera, Camera3)) {
                new_size = scale_viewport(new_size);
            }
            zox_set(camera, ScreenPosition, { position });
            zox_set(camera, ScreenDimensions, { new_size });
            if (dbg_log) {
                zox_log("+ Viewport Resize [%s] size [%ix%i] render camera? %i", zox_get_name(camera), new_size.x, new_size.y, zox_has(camera, RenderCamera));
            }
        }
    }
} zox_sys_end(ViewportResizeSystem);
