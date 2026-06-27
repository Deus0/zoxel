// todo: move to add key event

entity canvas_toggle_ui(ecs *world, entity canvas, entity e, entity (*spawn_ui)(ecs*, entity)) {
    if (e == 0) {
        return (*spawn_ui)(world, canvas);
    } else {
        zox_delete(e);
        return 0;
    }
}

zox_sys2(EditorInputSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CanvasLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CanvasLink, canvas);
        if (!zox_valid(canvas->value)) {
            continue;
        }
        entity e2 = zox_get_child_by_id(world, e, zox_id(Keyboard));
        if (!zox_valid(e2)) {
            continue;
        }
        if (zox_getv(e2, DeviceDisabled)) {
            continue;
        }
        zox_geter(e2, Keyboard, keyboard);
        if (keyboard->x.pressed_this_frame) {
            entity profiler = zox_get_child_by_id(world, canvas->value, zox_id(Profiler));
            canvas_toggle_ui(world, canvas->value, profiler, spawn_profiler_canvas);
        }
        else if (keyboard->c.pressed_this_frame) {
            entity label = zox_get_child_by_id(world, canvas->value, zox_id(GameDebugLabel));
            canvas_toggle_ui(world, canvas->value, label, spawn_game_debug_label);
        }
            //}
    }
} zox_sys_end(EditorInputSystem);
