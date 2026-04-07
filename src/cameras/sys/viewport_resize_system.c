zox_sys2(ViewportResizeSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(WindowSizeDirty);
    zox_sys_in(WindowSize);
    zox_sys_in(CameraLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(WindowSizeDirty, dirty);
        zox_sys_i(WindowSize, wsize);
        zox_sys_i(CameraLinks, cameras);

        if (!dirty->value) {
            continue;
        }
        //zox_log_error("viewport is a dirty [%i] cameras at [%ix%i]", cameras->length, wsize->value.x, wsize->value.y)
        // refresh viewport
        for (int j = 0; j < cameras->length; j++) {
            entity camera = cameras->value[j];

            if (!zox_valid(camera)) {
                continue;
            }

            // TODO: Camera should have CanvasLinks and a size dirty flag!

            float4 screen_to_canvas = zox_get_value(camera, ScreenToCanvas)
            int2 size = screen_to_canvas_size(wsize->value, screen_to_canvas);
            int2 position = screen_to_canvas_position(wsize->value, screen_to_canvas);

            if (zox_has(camera, Camera3D)) {
                size = scale_viewport(size);
            }
            /*  if (!zox_has(camera, RenderCamera)) {  }*/

            zox_set(camera, ScreenPosition, { position });
            zox_set(camera, ScreenDimensions, { size });

            // zox_logw("+ Viewport Resize [%s] size [%ix%i] render camera? %i", zox_get_name(camera), size.x, size.y, zox_has(camera, RenderCamera));
        }
    }
} zox_sys_end(ViewportResizeSystem);
