// For Elements of Square Shape - LayoutSize
// NOTE: Casts this from the Zevice (input device child)
zox_sys2(ElementRaycastSystem) {
    byte dbg_log = 0;
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZevicePointerPosition);
    zox_sys_out(EntityTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZevicePointerPosition, ray_position);
        zox_sys_o(EntityTarget, target);
        /*if (disabled->value) {
            raycaster_select_element(
                world,
                e,
                0);
            if (dbg_log >= 3) {
                zox_log("(ElementRaycastSystem) Zevice Disabled [%s]",
                    zox_getn(e));
            }
            continue;
        }*/
        entity device = zox_get_parent(world, e);
        if (!zox_valid(device)) {
            zox_loge("No Device owning Zevice [%s]",
                zox_getn(e));
            continue;
        }
        if (zox_has(device, Disabled)) {
            raycaster_select_element(world, e, 0);
            if (dbg_log >= 3) {
                zox_log("(ElementRaycastSystem) Zevice Disabled [%s]",
                    zox_getn(e));
            }
            continue;
        }
        entity player = zox_get_parent(world, device);
        if (!player) {
            zox_loge("No Player owning Device that was not disabled [%s]",
                zox_getn(device));
            continue;
        }
        byte dmode = zox_getv(player, DeviceMode);
        byte raycaster_mode =
            dmode == zox_device_mode_touchscreen ||
            (!keyboard_navigation_mode &&
            dmode == zox_device_mode_keyboardmouse);
        if (!raycaster_mode) {
            continue;
        }
        entity canvas = zox_get_link(
            world,
            player,
            CanvasLink);
        if (!zox_valid(canvas)) {
            continue;
        }
        entity camera = zox_get_link(
            world,
            canvas,
            CameraLink);
        if (!zox_valid(camera)) {
            continue;
        }
        float2 canvas_sizef = int2_to_float2(zox_getv(canvas, LayoutSize));
        float aspect_ratio = canvas_sizef.x / canvas_sizef.y;
        // NOTE: Now it only works for one canvas hmmm
        int2 canvas_position = zox_getv(camera, ScreenPosition);
        int2 canvas_size = zox_getv(camera, PixelSize);
        int2 position = ray_position->value;
        byte ray_in_viewport =
            position.x >= canvas_position.x &&
            position.x <= canvas_position.x + canvas_size.x &&
            position.y >= canvas_position.y &&
            position.y <= canvas_position.y + canvas_size.y;
        if (!ray_in_viewport) {
            if (dbg_log >= 2) {
                zox_log("(ElementRaycastSystem) Ray not in viewport [%s] at [%ix%i]",
                    zox_getn(e),
                    position.x,
                    position.y);
            }
            continue;
        }
        int ui_layer = -1;
        entity ui_selected = 0;
        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2();
            zox_sys_in_2(Position2);
            zox_sys_in_2(LayoutSize);
            zox_sys_in_2(Layer);
            zox_sys_in_2(RenderDisabled);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_i_2(RenderDisabled, disabled);
                zox_sys_i_2(Position2, position2);
                zox_sys_i_2(LayoutSize, lsize2);
                zox_sys_i_2(Layer, layer);
                if (disabled->value) {
                    continue;
                }
                entity e2 = it2.entities[j];
                entity rcanvas = zox_get_parent_by_id(world, e2, zox_id(Canvas));
                if (canvas != rcanvas) {
                    continue;
                }
                int2 lsize = lsize2->value;
                float2 p = position2->value;
                int2 viewport_position = {
                    canvas_position.x + (int)((p.x / aspect_ratio + 0.5f) * canvas_size.x),
                    canvas_position.y + (int)((p.y + 0.5f) * canvas_size.y)
                };
                // int2 viewport_position = canvasPosition2->value;
                // viewport_position.x += canvas_position.x;
                // viewport_position.y += canvas_position.y;
                // bounds should be offset with canvas position
                int4 ui_bounds = {
                    viewport_position.x - lsize.x / 2,
                    viewport_position.x + lsize.x / 2,
                    viewport_position.y - lsize.y / 2,
                    viewport_position.y + lsize.y / 2
                };
                if (zox_has(e2, MeshAlignment)) {
                    byte alignment = zox_getv(e2, MeshAlignment);
                    float4 scaler = alignment_to_scaler(alignment);
                    ui_bounds = (int4) {
                        viewport_position.x - lsize.x * scaler.x,
                        viewport_position.x + lsize.x * scaler.y,
                        viewport_position.y - lsize.y * scaler.z,
                        viewport_position.y + lsize.y * scaler.w
                    };
                }
                byte was_raycasted =
                    position.x >= ui_bounds.x &&
                    position.x <= ui_bounds.y &&
                    position.y >= ui_bounds.z &&
                    position.y <= ui_bounds.w;
                if (was_raycasted) {
                    if (layer->value > ui_layer) {
                        ui_layer = layer->value;
                        ui_selected = e2;
                        if (dbg_log) {
                            zox_log("[%s] was raycasted at [%x%ix%ix%i]",
                                zox_getn(e2),
                                ui_bounds.x,
                                ui_bounds.y,
                                ui_bounds.z,
                                ui_bounds.w);
                        }
                    }
                }
            }
        }
        zox_sys_query_end();
        // if only exists to block others (like Window's)
        if (ui_selected && !zox_has(ui_selected, SelectState)) {
            ui_selected = 0;
        }
        if (target->value != ui_selected) {
            raycaster_select_element(
                world,
                e,
                ui_selected);
        }
    }
} zox_sys_end(ElementRaycastSystem);
