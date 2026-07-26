// For Elements of Square Shape - LayoutSize
zox_sys2(ElementRaycastSystem) {
    byte dbg_log = 0;
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Raycaster);
    zox_sys_in(DeviceLink);
    zox_sys_out(RaycasterTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceLink, device);
        zox_sys_i(Raycaster, raycaster);
        zox_sys_o(RaycasterTarget, target);
        if (!device->value) {
            continue;
        }
        if (zox_gett_value(device->value, DeviceDisabled)) {
            continue;
        }
        entity player = zox_get_parent(world, device->value);
        if (!player) {
            continue;
        }
        zox_geter_value(player, DeviceMode, byte, dmode);
        byte dmode_raycaster = dmode == zox_device_mode_touchscreen || (!keyboard_navigation_mode && dmode == zox_device_mode_keyboardmouse);
        if (!dmode_raycaster) {
            continue;
        }
        zox_geter_value(player, CanvasLink, entity, pcanvas);
        if (!zox_valid(pcanvas)) {
            continue;
        }
        zox_geter_value(pcanvas, CameraLink, entity, camera);
        if (!zox_valid(camera)) {
            continue;
        }
        float2 canvas_sizef = int2_to_float2(zox_getv(pcanvas, LayoutSize));
        float aspect_ratio = canvas_sizef.x / canvas_sizef.y;
        // NOTE: Now it only works for one canvas hmmm
        int2 canvas_position = zox_getv(camera, ScreenPosition);
        int2 canvas_size = zox_getv(camera, ScreenDimensions);
        int2 position = raycaster->value;
        byte ray_in_viewport =
            position.x >= canvas_position.x &&
            position.x <= canvas_position.x + canvas_size.x &&
            position.y >= canvas_position.y &&
            position.y <= canvas_position.y + canvas_size.y;
        if (!ray_in_viewport) {
            continue;
        }
        int ui_layer = -1;
        entity ui_selected = 0;
        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2();
            zox_sys_in_2(Position2);
            zox_sys_in_2(LayoutSize);
            zox_sys_in_2(Layer2D);
            zox_sys_in_2(RenderDisabled);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_i_2(RenderDisabled, rdisabled);
                zox_sys_i_2(Position2, position2);
                zox_sys_i_2(LayoutSize, lsize2);
                zox_sys_i_2(Layer2D, layer2D);
                if (rdisabled->value) {
                    continue;
                }
                entity e2 = it2.entities[j];
                entity rcanvas = zox_get_parent_by_id(world, e2, zox_id(Canvas));
                if (pcanvas != rcanvas) {
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
                    if (layer2D->value > ui_layer) {
                        ui_layer = layer2D->value;
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
            raycaster_select_element(world, e, ui_selected);
        }
    }
} zox_sys_end(ElementRaycastSystem);
