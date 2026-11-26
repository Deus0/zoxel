// For Elements of Square Shape - LayoutSize
// NOTE: Casts this from the Zevice (input device child)
void element_raycast_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZevicePointerPosition);
    zox_sys_out(Target);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZevicePointerPosition, ray_position);
        zox_sys_o(Target, target);
        /*if (disabled->value) {
            raycaster_select_element(
                world,
                e,
                0);
            if (dbg_log >= 3) {
                zox_log("(element_raycast_system) Zevice Disabled [%s]",
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
                zox_log("(element_raycast_system) Zevice Disabled [%s]",
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
        // its canvases (CanvasLink, many): its own (its PlayerLink is the player), and one it shares with every player
        //  (the pause menu's, over the whole window): what's hit on that one is over its own
        entity canvas = 0, shared_canvas = 0;
        for (int k = 0; ; k++) {
            const entity c = zox_get_link_index(world, player, CanvasLink, k);
            if (!c) {
                break;
            }
            if (zox_get_link(world, c, PlayerLink) == player) {
                canvas = c;
            } else {
                shared_canvas = c;
            }
        }
        if (!zox_valid(canvas)) {
            continue;
        }
        CanvasViewport viewport;
        if (!get_canvas_viewport(world, canvas, &viewport)) {
            continue;
        }
        int2 position = ray_position->value;
        byte ray_in_viewport = is_in_canvas_viewport(&viewport, position);
        if (!ray_in_viewport) {
            // off the canvas (out of the window): nothing's hovered (it kept its target, its tooltip stuck)
            if (target->value) {
                raycaster_select_element(world, e, 0);
            }
            if (dbg_log >= 2) {
                zox_log("(element_raycast_system) Ray not in viewport [%s] at [%ix%i]",
                    zox_getn(e),
                    position.x,
                    position.y);
            }
            continue;
        }
        int ui_layer = -1;
        entity ui_selected = 0;
        CanvasViewport shared_viewport;
        const byte has_shared = zox_valid(shared_canvas) && get_canvas_viewport(world, shared_canvas, &shared_viewport);
        int shared_layer = -1;
        entity shared_selected = 0;
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
                const byte on_shared = has_shared && rcanvas == shared_canvas;
                if (canvas != rcanvas && !on_shared) {
                    continue;
                }
                const CanvasViewport* active_viewport = on_shared ? &shared_viewport : &viewport;
                int4 ui_bounds = get_element_pointer_bounds(
                    world,
                    active_viewport,
                    e2,
                    position2->value,
                    lsize2->value);
                const int4* clip = get_ui_clip(world, e2);
                if (clip) {
                    const int2 origin = active_viewport->position;
                    const int4 bounds = { clip->x + origin.x, clip->y + origin.x,
                        clip->z + origin.y, clip->w + origin.y };
                    if (position.x >= bounds.y || position.y >= bounds.w) {
                        continue;
                    }
                    ui_bounds = intersect_layout_clips(ui_bounds, bounds);
                }
                byte was_raycasted = is_in_pointer_bounds(ui_bounds, position);
                if (was_raycasted && on_shared) {
                    if (layer->value > shared_layer) {
                        shared_layer = layer->value;
                        shared_selected = e2;
                    }
                } else if (was_raycasted) {
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
        if (shared_selected) {
            ui_selected = shared_selected;
        }
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
    zox_sys_on_end();
} zoxd_system(element_raycast_system);
