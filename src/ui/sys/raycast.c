// For Elements of Square Shape - LayoutSize
zox_sys2(ElementRaycastSystem) {
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Raycaster);
    zox_sys_in(DeviceLink);
    zox_sys_out(RaycasterTarget);
    zox_sys_out(WindowRaycasted);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceLink, deviceLink);
        zox_sys_i(Raycaster, raycaster);
        zox_sys_o(RaycasterTarget, raycasterTarget);
        zox_sys_o(WindowRaycasted, windowRaycasted);

        if (!deviceLink->value) {
            continue;
        }

        if (zox_gett_value(deviceLink->value, DeviceDisabled)) {
            continue;
        }

        zox_geter_value(deviceLink->value, PlayerLink, entity, player);
        if (!player) {
            continue;
        }

        zox_geter_value(player, DeviceMode, byte, dmode);

        byte dmode_raycaster =
            dmode == zox_device_mode_touchscreen ||
            (!keyboard_navigation_mode && dmode == zox_device_mode_keyboardmouse);

        if (!dmode_raycaster) {
            continue;
        }

        entity player_canvas = zox_get_value(player, CanvasLink)
        entity player_camera_ui = zox_gett_value(player_canvas, CameraLink);
        int2 position = raycaster->value;
        int ui_layer = -1;
        entity ui_selected = 0;
        int window_layer = -1;
        entity window_selected = 0;

        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2();
            zox_sys_in_2(CanvasPosition);
            zox_sys_in_2(LayoutSize);
            zox_sys_in_2(Layer2D);
            zox_sys_in_2(RenderDisabled);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_i_2(RenderDisabled, rdisabled);
                zox_sys_i_2(CanvasPosition, canvasPosition2);
                zox_sys_i_2(LayoutSize, lsize2);
                zox_sys_i_2(Layer2D, layer2D);

                if (rdisabled->value) {
                    continue;
                }

                entity e2 = it2.entities[j];
                entity camera = zox_get_root_canvas_camera(world, e2);
                if (!camera) {
                    continue;
                }

                if (player_camera_ui != camera) {
                    continue; // only do checks for player canvases
                }

                int2 lsize = lsize2->value;
                int2 canvas_position = zox_get_value(camera, ScreenPosition);
                int2 canvas_size = zox_get_value(camera, ScreenDimensions);

                byte ray_in_viewport = position.x >= canvas_position.x && position.x <= canvas_position.x + canvas_size.x && position.y >= canvas_position.y && position.y <= canvas_position.y + canvas_size.y;

                if (!ray_in_viewport) {
                    continue;
                }

                int2 viewport_position = canvasPosition2->value;
                viewport_position.x += canvas_position.x;
                viewport_position.y += canvas_position.y;
                // bounds should be offset with canvas position
                int4 ui_bounds = {
                    viewport_position.x - lsize.x / 2,
                    viewport_position.x + lsize.x / 2,
                    viewport_position.y - lsize.y / 2,
                    viewport_position.y + lsize.y / 2
                };

                if (zox_has(e2, MeshAlignment)) {
                    zox_geter_value(e2, MeshAlignment, byte, alignment);
                    float4 scaler = alignment_to_scaler(alignment);
                    ui_bounds = (int4) {
                        viewport_position.x - lsize.x * scaler.x,
                        viewport_position.x + lsize.x * scaler.y,
                        viewport_position.y - lsize.y * scaler.z,
                        viewport_position.y + lsize.y * scaler.w
                    };
                }

                byte was_raycasted = position.x >= ui_bounds.x && position.x <= ui_bounds.y && position.y >= ui_bounds.z && position.y <= ui_bounds.w;

                if (was_raycasted) {
                    byte window_raycasted = zox_has(e2, WindowRaycastTarget);

                    if (layer2D->value > ui_layer) { // !window_raycasted &&
                        ui_layer = layer2D->value;
                        ui_selected = e2;
                    }

                    if (window_raycasted && layer2D->value > window_layer) {
                        if (!zox_has(e2, Window)) {
                            // if header/body use parent
                            // window_selected = zox_get_value(e2, ParentLink);
                            window_selected = zox_get_parent(world, e2);
                            window_layer = zox_get_value(ui_selected, Layer2D);
                        } else {
                            window_selected = e2;
                            window_layer = layer2D->value;
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

        if (raycasterTarget->value != ui_selected) {
            raycaster_select_element(world, e, ui_selected);
        }

        if (windowRaycasted->value != window_selected) {
            raycaster_select_window(world, e, window_selected);
        }
    }
} zox_sys_end(ElementRaycastSystem);
