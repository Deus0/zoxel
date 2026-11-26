// n: toggles the file window on the debug player's canvas.
void toggle_files(ecs* world, int32_t keycode) {
    if (keycode != zox_key_n) {
        return;
    }
    const entity canvas = get_editor_canvas(world);
    if (!canvas) {
        return;
    }
    const entity window = zox_get_child_by_id(world, canvas, zox_id(FileWindowUI));
    if (zox_valid(window)) {
        zox_delete(window);
        play_editor_toggle_sound(world, 0);
    } else {
        spawn_window_files(world, canvas, dbg_player, NULL);
        play_editor_toggle_sound(world, 1);
    }
}
