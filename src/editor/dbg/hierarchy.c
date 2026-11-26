// y: toggles the hierarchy, of every root entity (the scene, not the editor's ui)
void toggle_hierarchy(ecs *world, int32_t keycode) {
    if (keycode != zox_key_y) {
        return;
    }
    const entity canvas = get_editor_canvas(world);
    if (!canvas) {
        return;
    }
    const entity hierarchy = zox_get_child_by_id(world, canvas, zox_id(HierarchyUI));
    if (zox_valid(hierarchy)) {
        dock_close(world, canvas, hierarchy);
        play_editor_toggle_sound(world, 0);
        return;
    }
    spawn_window_hierarchy(world, canvas, dbg_player, 0);   // every root
    play_editor_toggle_sound(world, 1);
}
