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
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity canvas = zox_get_link(world, e, CanvasLink);
        if (!zox_valid(canvas)) {
            continue;
        }
        entity e2 = zox_get_child_by_id(world, e, zox_id(Keyboard));
        if (zox_has(e2, Disabled)) {
            continue;
        }
        zox_geter(e2, Keyboard, keyboard);
        if (keyboard->x.pressed_this_frame) {
            entity profiler = zox_get_child_by_id(world, canvas, zox_id(Profiler));
            canvas_toggle_ui(world, canvas, profiler, spawn_profiler_canvas);
        } else if (keyboard->c.pressed_this_frame) {
            entity label = zox_get_child_by_id(world, canvas, zox_id(GameDebugLabel));
            canvas_toggle_ui(world, canvas, label, spawn_game_debug_label);
        }
    }
} zox_sys_end(EditorInputSystem);
