// todo: move to add key event
zox_sys2(EditorInputSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceLinks);
    zox_sys_in(CanvasLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceLinks, devices);
        zox_sys_i(CanvasLink, canvas);
        if (!zox_valid(canvas->value)) {
            continue;
        }
        for (int j = 0; j < devices->length; j++) {
            entity device = devices->value[j];
            if (!zox_valid(device) || zox_gett_value(device, DeviceDisabled)) {
                continue;
            }
            if (zox_has(device, Keyboard)) {
                zox_geter(device, Keyboard, keyboard);
                // toggle uis
                if (keyboard->x.pressed_this_frame) {
                    entity profiler = zox_get_child_by_id(world, canvas->value, zox_id(Profiler));
                    player_toggle_ui(world, e, profiler, spawn_profiler_canvas);
                }
                else if (keyboard->c.pressed_this_frame) {
                    entity label = zox_get_child_by_id(world, canvas->value, zox_id(GameDebugLabel));
                    player_toggle_ui(world, e, label, spawn_profiler_canvas);
                }
                /*else if (keyboard->v.pressed_this_frame) {
                    entity profiler = zox_get_child_by_id(world, canvas->value, zox_id(Profiler));
                    if (!zox_valid(profiler)) {
                        zox_logw("Profiler not found.");
                        continue;
                    }
                    zox_geter(e, Children, plots);
                    for (int j = 0; j < plots->length; j++) {
                        entity plot = plots->value[j];
                        if (!zox_has(plot, PlotPaused)) {
                            zox_log("   - not plot [%lu]", plot);
                            continue;
                        }
                        byte is_pause = !zox_gett_value(plot, PlotPaused);
                        zox_set(plot, PlotPaused, { is_pause });
                        // zox_log("plot [%lu] is %s.", e, is_pause ? "paused" : "running");
                    }
                }*/
            }
        }
    }
} zox_sys_end(EditorInputSystem);
