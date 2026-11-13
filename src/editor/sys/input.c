// Editor keys (x: profiler, c: debug label), the other tools are toggled with key down hooks
// TODO: move these to add_hook_key_down too

void editor_input_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity canvas = zox_get_link(world, e, CanvasLink);
        const entity keyboard_device = zox_get_child_by_id(world, e, zox_id(Keyboard));
        if (!zox_valid(canvas) ||
            !zox_valid(keyboard_device) ||
            zox_has(keyboard_device, Disabled))
        {
            continue;
        }
        const Keyboard* keyboard = zox_get(keyboard_device, Keyboard);
        if (keyboard->x.pressed_this_frame) {
            const entity profiler = zox_get_child_by_id(world, canvas, zox_id(Profiler));
            canvas_toggle_ui(world, canvas, profiler, spawn_profiler_canvas);
        } else if (keyboard->c.pressed_this_frame) {
            const entity label = zox_get_child_by_id(world, canvas, zox_id(GameDebugLabel));
            canvas_toggle_ui(world, canvas, label, spawn_game_debug_label);
        }
    }
    zox_sys_on_end();
} zoxd_system(editor_input_system);
