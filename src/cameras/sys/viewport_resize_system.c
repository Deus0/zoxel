zox_sys2(ViewportResizeSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(WindowSizeDirty);
    zox_sys_in(WindowSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(WindowSizeDirty, dirty);
        zox_sys_i(WindowSize, size);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (dbg_log) {
            zox_log("Viewport Dirty at [%ix%i]", size->value.x, size->value.y);
        }
        uint j = 0;
        while (1) {
            entity camera = zox_get_link_index(world, e, ViewportCamera, j);
            if (!camera) {
                break;
            }
            j++;
        /*iter it2 = zox_links(e, ViewportCamera);
        while (zox_query_next(it2)) {
            entity camera = zox_pair_target(it2, 0);
            if (!zox_valid(camera)) {
                continue;
            }*/
            // TODO: Camera should have CanvasLinks and a size dirty flag!
            float4 screen_to_canvas = zox_getv(camera, ScreenToCanvas);
            int2 new_size = screen_to_canvas_size(size->value, screen_to_canvas);
            int2 position = screen_to_canvas_position(size->value, screen_to_canvas);
            if (zox_has(camera, Camera3)) {
                new_size = scale_viewport(new_size);
            }
            zox_setv(camera, ScreenPosition, position);
            zox_setv(camera, PixelSize, new_size);
            if (dbg_log) {
                zox_log("+ Viewport Resize [%s] size [%ix%i] render camera? %i",
                    zox_get_name(camera),
                    new_size.x,
                    new_size.y,
                    zox_has(camera, RenderCamera));
            }
        }
        //}
    }
} zox_sys_end(ViewportResizeSystem);
